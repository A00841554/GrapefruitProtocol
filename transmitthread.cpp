#include "receivethread.h"
#include "transmitthread.h"

DWORD WINAPI fnTransmitIdle(LPVOID lpArg)
{
    TransmitArgs* pTransmit = (TransmitArgs*) lpArg;

    // reset state to delay sending ENQ
    if(pTransmit->bReset)
    {
        OutputDebugString("TransmitThread: Reset\n");
        Sleep(rand() % (MAX_RESET_TIMEOUT - MIN_RESET_TIMEOUT) + MIN_RESET_TIMEOUT);
        pTransmit->bReset = false;
    }

    // Reset SYN1
    pTransmit->bSYN1 = true;

    // check for data to send, or a request to stop
    OutputDebugString("TransmitThread: Idle\n");
    HANDLE handles[] = {pTransmit->hRequestStop, pTransmit->hRequestActive};
    DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    switch(result)
    {
        case WAIT_OBJECT_0+0:
        {
            _TransmitThread_::fnStop(pTransmit);
            break;
        }
        case WAIT_OBJECT_0+1:
        {
            if(!pTransmit->pReceive->bActive)
                _TransmitThread_::fnGoActive(pTransmit);
            else
                _TransmitThread_::fnStop(pTransmit);
            break;
        }
        default:
        {
            DWORD err = GetLastError();
            OutputDebugString("Something went wrong...\n");
            _TransmitThread_::fnStop(pTransmit);
            break;
        }
    }

    OutputDebugString("TransmitThread: Stopped\n");
    return 0;
}

DWORD WINAPI fnTransmitActive(LPVOID lpArg)
{
    OutputDebugString("TransmitThread: Active\n");

    TransmitArgs* pTransmit = (TransmitArgs*) lpArg;
    char byReceivedChar = 0;
    short nPacketsSent = 0;
    short nPacketsMiss;
    char* pSCurrPacket;

    // stop the receive thread
    SetEvent(pTransmit->pReceive->hRequestStop);

    // wait for receive thread to stop before going full active
    while(!pTransmit->pReceive->bStopped)
    {
        Sleep(SHORT_SLEEP);
    }
    OutputDebugString("TransmitThread: Full Active\n");

    // bid for the line; send an ENQ or an RVI
    if(pTransmit->pReceive->bRVI)
    {
        fnSendData(RVI, pTransmit->hCommPort);
    }
    else
    {
        fnSendData(ENQ, pTransmit->hCommPort);
    }

    // wait for ACK before transmitting; if we fail (usually by timing out),
    // bail out
    int result = fnWaitForChar(pTransmit->hCommPort, ACK, TIMEOUT_AFTER_T_ENQ);
    if(result != ReadDataResult::SUCCESS)
    {
        _TransmitThread_::fnReset(pTransmit);
        return 0;
    }

    while(true)
    {
        char pSCurrPacket[PACKET_SIZE];
        fnPacketizeData(*pTransmit, pSCurrPacket, nPacketsSent >= MAX_SENT);

        nPacketsMiss = 0;
        nPacketsSent++;

        while(true)
        {
            // clear rvi
            pTransmit->pReceive->bRVI = false;

            // send data
            fnSendData(pSCurrPacket, pTransmit->hCommPort);
            char expectedChars[] = {ACK, NAK, RVI};
            int result = fnWaitForChars(pTransmit->hCommPort, &byReceivedChar,
                        expectedChars, sizeof(expectedChars),
                        TIMEOUT_AFTER_T_PACKET);

            // we received an ack, discard the sent data because we don't need to retransmit it anymore
            if(result != ReadDataResult::TIMEDOUT && (byReceivedChar == ACK || byReceivedChar == RVI))
            {
                OutputDebugString("TransmitThread: Transmit Packet\n");

                // update stats, and the UI
                fnUpdateStats(STATS_PCKT_SENT);
                fnSentData(pSCurrPacket);
                if(fnIsEOT(pSCurrPacket))
                    fnSentData("HH\r\n----------------------\r\n");

                // sent data has been acked; remove it & if there are no more data to send, reset our
                // request to go active flag
                fnDropHeadPacketData(pTransmit);
                if (pTransmit->pTransmitBuffer->empty())
                {
                    ResetEvent(pTransmit->hRequestActive);
                }
            }

            // check for reset conditions
            if ((result == ReadDataResult::TIMEDOUT && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == NAK && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == ACK && fnIsEOT(pSCurrPacket)))
            {
                _TransmitThread_::fnReset(pTransmit);
                return 0;   // gtfo we're done here
            }

            // check for retransmit conditions
            else if ((result == ReadDataResult::TIMEDOUT || byReceivedChar == NAK) && nPacketsMiss < MAX_MISS)
            {
                nPacketsMiss++;
                continue;   // retransmit
            }

            // break out of retransmission loop, and transmit the next packet
            else if (byReceivedChar == ACK && fnIsETB(pSCurrPacket))
            {
                break;      // transmit next packet
            }

            // check for RVI condition
            else if (byReceivedChar == RVI)
            {
                _TransmitThread_::fnStop(pTransmit);
                _ReceiveThread_::fnGoActive(pTransmit->pReceive);
                return 0;   // gtfo we're done here
            }
        }
    }
}

void _TransmitThread_::fnGoActive(TransmitArgs* pTransmit)
{
    SetEvent(pTransmit->pReceive->hRequestStop);
    pTransmit->bActive = true;
    fnTransmitActive(pTransmit);
}

void _TransmitThread_::fnReset(TransmitArgs* pTransmit)
{
    _TransmitThread_::fnStop(pTransmit);
    pTransmit->bReset = true;
}

void _TransmitThread_::fnStop(TransmitArgs* pTransmit)
{
    pTransmit->bActive = false;
    pTransmit->bStopped = true;
}
