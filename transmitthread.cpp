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

    
    
    for(;;)
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

    while(byReceivedChar != 6) // ACK 6 6 6 ^F Acknowledge, clears ENQ
    {
        dwBytesRead = 0;
        ReadFile(pTransmit->hCommPort, &byReceivedChar, 1, &dwBytesRead, NULL);
        if (dwBytesRead > 0 &&
    }
}














