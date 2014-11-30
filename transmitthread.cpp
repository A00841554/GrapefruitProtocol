#include "transmitthread.h"
#include "helper.h"

DWORD WINAPI fnTransmitIdle(LPVOID lpArg)
{
    OutputDebugString("TransmitThread: Started\n");

    using namespace std;
    srand(time(NULL));

	TransmitArgs* pTransmit = (TransmitArgs*) lpArg;
	
    if(pTransmit->bReset)
    {
        Sleep(rand() % MAX_RESET_TIMEOUT);
        pTransmit->bReset = false;
    }

    
    
    while(true)
    {
        if(pTransmit->bRequestStop)
        {
            OutputDebugString("TransmitThread: Stopped\n");
            pTransmit->bStopped = true;
            return 0;
        }

        if(!pTransmit->pTransmitBuffer->empty()) 
        {
            break;
        }
        
        Sleep(SHORT_SLEEP);
    }

    if(pTransmit->bActive)
        pTransmit->bStopped = true;
    else 
    {
        pTransmit->pReceive->bRequestStop = true;
        pTransmit->bActive = true;
        fnTransmitActive(pTransmit);
    }

	OutputDebugString("TransmitThread: Stopped\n");
	return 0;
}

DWORD WINAPI fnTransmitActive(LPVOID lpArg)
{
    OutputDebugString("TransmitThread: Active\n");

	TransmitArgs* pTransmit = (TransmitArgs*) lpArg;
	char byReceivedChar = 0;
    DWORD dwBytesRead;
    short nPacketsSent;
    short nPacketsMiss;
    char* pSCurrPacket;

    pTransmit->pReceive->bRequestStop = true;
	
	if(pTransmit->pReceive->bRVI)
	{
		fnSendData(RVI, (*pTransmit->pHCommPort));
		pTransmit->pReceive->bRVI = false;
	}
	else
	{
		fnSendData(ENQ, (*pTransmit->pHCommPort));
	}

    while(byReceivedChar != ACK)
    {
        dwBytesRead = 0;
        int result = fnReadData(*(pTransmit->pHCommPort), &byReceivedChar, 1, TIMEOUT_AFTER_T_ENQ);
        if (result != ReadDataResult::SUCCESS)
        {
            pTransmit->bActive = false;
            pTransmit->bStopped = true;
            pTransmit->bReset = true;
            return 0;
        }
    }

    nPacketsSent = 0;

    while(true)
    {
        char pSCurrPacket[PACKET_SIZE];
        fnPacketizeData(*pTransmit, pSCurrPacket, nPacketsSent >= MAX_SENT);
        fnDropHeadPacketData(*pTransmit);
        
        nPacketsMiss = 0;
        nPacketsSent++;

        while(true)
        {
            fnSendData(pSCurrPacket, *(pTransmit->pHCommPort));
            dwBytesRead = 0;
            int result = fnReadData(*(pTransmit->pHCommPort), &byReceivedChar, 1, TIMEOUT_AFTER_T_PACKET);

            // if we missed too many times, we want to start transmitting where we left off; add the packet
            // back to the head of our transmit buffer
            if (result == ReadDataResult::TIMEDOUT && nPacketsMiss >= MAX_MISS)
            {
                fnAddHeadPacketData(*pTransmit, pSCurrPacket);
            }

            // check for conditions to exit from the transmit threads
            if ((result == ReadDataResult::TIMEDOUT && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == NAK && nPacketsMiss >= MAX_MISS) ||
                (result != ReadDataResult::TIMEDOUT && byReceivedChar == ACK && fnIsEOT(pSCurrPacket)))
            {
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                pTransmit->bReset = true;
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
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                return 0;
            }
        }
    }
}