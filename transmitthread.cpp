#include "transmitthread.h"

DWORD WINAPI fnTransmitIdle(LPVOID lpArg)
{
    using namespace std;
    srand(time(NULL));

    TransmitArgs* pTransmit = (TransmitArgs*) lpArg;

    // reset state to delay sending ENQ
    if(pTransmit->bReset)
    {
        OutputDebugString("TransmitThread: Reset\n");
        Sleep(rand() % MAX_RESET_TIMEOUT);
        pTransmit->bReset = false;
    }

    // check for data to send, or a request to stop
    OutputDebugString("TransmitThread: Idle\n");
    while(true)
    {
        if(pTransmit->bRequestStop)
        {
            _TransmitThread_::fnStop(pTransmit);
            break;
        }

        if(!pTransmit->pTransmitBuffer->empty()) 
        {
            _TransmitThread_::fnGoActive(pTransmit);
            break;
        }
        
        Sleep(SHORT_SLEEP);
    }

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

    pTransmit->pReceive->bRequestStop = true;

    // bid for the line; send an ENQ or an RVI
    if(pTransmit->pReceive->bRVI)
    {
        fnSendData(RVI, (*pTransmit->pHCommPort));
        pTransmit->pReceive->bRVI = false;
    }
    else
    {
        fnSendData(ENQ, *pTransmit->pHCommPort);
    }

    // wait for ACK before transmitting; if we fail (usually by timing out),
    // bail out
    int result = fnWaitForChar(*pTransmit->pHCommPort, ACK, TIMEOUT_AFTER_T_ENQ);
    if(result != ReadDataResult::SUCCESS)
    {
        _TransmitThread_::fnReset(pTransmit);
        return 0;
    }

    while(true)
    {
        char pSCurrPacket[PACKET_SIZE];
        fnPacketizeData(*pTransmit, pSCurrPacket, nPacketsSent >= MAX_SENT);
        fnDropHeadPacketData(pTransmit);

        nPacketsMiss = 0;
        nPacketsSent++;

        while(true)
        {
            fnSendData(pSCurrPacket, *(pTransmit->pHCommPort));
            char expectedChars[] = {ACK, NAK, RVI};
            int result = fnWaitForChars(*pTransmit->pHCommPort, &byReceivedChar,
                        expectedChars, sizeof(expectedChars),
                        TIMEOUT_AFTER_T_PACKET);

            // if we missed too many times, we want to start transmitting where we left off; add the packet
            // back to the head of our transmit buffer
            if (result == ReadDataResult::TIMEDOUT && nPacketsMiss >= MAX_MISS)
            {
                fnAddHeadPacketData(pTransmit, pSCurrPacket);
            }

            // check for reset conditions
            if ((result == ReadDataResult::TIMEDOUT && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == NAK && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == ACK && fnIsEOT(pSCurrPacket)))
            {
                _TransmitThread_::fnReset(pTransmit);
                return 0;
            }

            // check for retransmit conditions
            else if ((result == ReadDataResult::TIMEDOUT || byReceivedChar == NAK) && nPacketsMiss < MAX_MISS)
            {
                nPacketsMiss++;
                continue;
            }

            // break out of retransmission loop, and transmit the next packet
            else if (byReceivedChar == ACK && fnIsETB(pSCurrPacket))
            {
                break;
            }

            // check for RVI condition
            else if (byReceivedChar == RVI)
            {
                _TransmitThread_::fnStop(pTransmit);
                return 0;
            }
        }
    }
}

void _TransmitThread_::fnGoActive(TransmitArgs* pTransmit)
{
    pTransmit->pReceive->bRequestStop = true;
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
    OutputDebugString("TransmitThread: Stopped\n");
    pTransmit->bActive = false;
    pTransmit->bStopped = true;
}

