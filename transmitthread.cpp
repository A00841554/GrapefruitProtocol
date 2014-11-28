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
        //Sleep(rand())
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
        pTransmit->bActive = true;
        fnTransmitActive(pTransmit);

        OutputDebugString("TransmitThread: Stopped\n");
        return 0;
    }
}

DWORD WINAPI fnTransmitActive(LPVOID lpArg)
{
    OutputDebugString("TransmitThread: Active\n");

	TransmitArgs* pTransmit = (TransmitArgs*) lpArg;
	unsigned char byReceivedChar = 0;
    DWORD dwBytesRead;
    short nPacketsSent;
    short nPacketsMiss;
    char* pSCurrPacket;

    pTransmit->pReceive->bRequestStop = true;
	
	if(pTransmit->pReceive->bRVI)
	{
		//TODO
		fnSendData(RVI, (*pTransmit->pHCommPort));
		pTransmit->pReceive->bRVI = false;
	}
	else
	{
		//TODO
		fnSendData(ENQ, (*pTransmit->pHCommPort));
	}

    while(byReceivedChar != ACK)
    {
        dwBytesRead = 0;
        ReadFile((*pTransmit->pHCommPort), &byReceivedChar, 1, &dwBytesRead, NULL);
        if (dwBytesRead == 0)
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
        //TODO
        // "Add || maybe" - Eric
        pSCurrPacket = fnPacketizeData(*pTransmit, nPacketsSent >= MAX_SENT);
        
        nPacketsMiss = 0;
        nPacketsSent++;

        while(true)
        {
            //TODO
            fnSendData(pSCurrPacket);
            dwBytesRead = 0;
            ReadFile(*(pTransmit->pHCommPort), &byReceivedChar, 1, &dwBytesRead, NULL);
            if ((dwBytesRead == 0 && nPacketsMiss >= MAX_MISS) ||
                (byReceivedChar == NAK && nPacketsMiss >= MAX_MISS) ||

                (byReceivedChar == ACK && fnIsEOT(pSCurrPacket)))
            {
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                pTransmit->bReset = true;
                return 0;
            }
            else if ((dwBytesRead == 0 || byReceivedChar == NAK) && nPacketsMiss < MAX_MISS)
            {
                nPacketsMiss++;
                continue;
            }

            else if (byReceivedChar == ACK && fnIsETB(pSCurrPacket))
            {
                break;
            }
            else if (byReceivedChar == RVI)
            {
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                return 0;
            }
        }
    }
}