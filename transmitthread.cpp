#include "transmitthread.h"

DWORD WINAPI fnTransmitIdle(LPVOID lpArg)
{
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
            pTransmit->bStopped = true;
            return;
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

        return;
    }
}

DWORD WINAPI fnTransmitActive(LPVOID lpArg)
{
	TransmitArgs* pTransmit = (TransmitArgs*) lpArg;
	unsigned char byReceivedChar;
    DWORD dwBytesRead;
    short nPacketsSent;
    short nPacketsMiss;
    char* pSCurrPacket;

    pTransmit->pReceive->bRequestStop = true;
	
	if(pTransmit->pReceive->bRVI)
	{
		//TODO
		//fnSendData(RVI);
		pTransmit->pReceive->bRVI = false;
	}
	else
	{
		//TODO
		//fnSendData(ENQ);
	}

    while(byReceivedChar != ACK) // ACK 6 6 6 ^F Acknowledge, clears ENQ
    {
        dwBytesRead = 0;
        ReadFile(pTransmit->hCommPort, &byReceivedChar, 1, &dwBytesRead, NULL);
        if (dwBytesRead == 0)
        {
            pTransmit->bActive = false;
            pTransmit->bStopped = true;
            pTransmit->bReset = true;
            return;
        }
    }

    nPacketsSent = 0;

    while(true)
    {
        //TODO
        // "Add || maybe" - Eric
        //pSCurrPacket = fnPacketizeData(pTransmit, nPacketsSent >= MAX_SENT);
        
        nPacketsMiss = 0;
        nPacketsSent++;

        while(true)
        {
            //fnSendData(pSCurrPacket);
            dwBytesRead = 0;
            ReadFile(pTransmit->hCommPort, &byReceivedChar, 1, &dwBytesRead, NULL);
            if (dwBytesRead == 0 && nPacketsMiss >= MAX_MISS)
            {
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                pTransmit->bReset = true;
                return;
            }
            else if ((dwBytesRead == 0 || byReceivedChar == NAK) && nPacketsMiss < MAX_MISS)
            {
                nPacketsMiss++;
                continue;
            }
            else if (byReceivedChar == ACK /* && fnIsETB(pSCurrPacket)*/)
            {
                break;
            }
            else if (byReceivedChar == RVI)
            {
                pTransmit->bActive = false;
                pTransmit->bStopped = true;
                return;
            }
        }
    }
}