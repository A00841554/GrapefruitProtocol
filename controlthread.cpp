#include "controlthread.h"

fnControl()
{
    // create transmit and receive structures
    TransmitArgs transmitArgs;
    ReceiveArgs receiveArgs;

    // initialize transmit structure
    transmitArgs.bRequestStop = false;
    transmitArgs.bStopped     = true;
    transmitArgs.bActive      = false;
    transmitArgs.bReset       = false;
    transmitArgs.bSYN1        = true;
    transmitArgs.pReceive     = &receiveArgs;

    // initialize receive thread structures
    receiveArgs.bRequestStop = false;
    receiveArgs.bStopped     = true;
    receiveArgs.bActive      = false;
    receiveArgs.bRVI         = false;
    receiveArgs.bSYN1        = false;
    receiveArgs.pTransmit    = &transmitArgs;

    // enter main control loop
    while (true)
    {
        // restart any threads that are stopped if the state of both threads are
        // not active
        if (!receiveArgs.bActive && !transmitArgs.bActive)
        {
            if (receiveArgs.bStopped)
            {
                receiveArgs.bRequestStop = false;
                receiveArgs.bStopped = false;
                LPDWORD threadId;
                CreateThread(, NULL, fnReceiveThreadIdle, &receiveArgs, 0, &threadId);
            }
            if (transmitArgs.bStopped)
            {
                transmitArgs.bRequestStop = false;
                transmitArgs.bStopped = false;
            }
        }
    }
}

/*
#include <iostream>
int main(void) {
    ReceiveArgs a;
    a.bRequestStop = false;
    a.bStopped = false;
    a.bActive = false;
    std::cout << a.bActive << std::endl;
    return 0;
}
*/
