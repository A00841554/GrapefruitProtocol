#include "transmitthread.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

void fnTransmitIdle(TransmitArgs* pTransmit)
{
    using namespace std;
    srand(time(NULL));

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

void fnTransmitActive(TransmitArgs* pTransmit)
{
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
	
	While(
}














