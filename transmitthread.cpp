#include "transmitthread.h"
#include <cstdlib>
#include <ctime>

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

        /*Wait for event from the transmit buffer

        if(waiting times-out) {

            continue

        }

        if(event from transmit buffer arrives) {

            exit loop

        }

    if(transmit.receive.active == true) {

        transmit.stopped = true

    } else {

        transmit.active = true

        call TransmitActive(transmit)

    return*/
    }
}