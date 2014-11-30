#include "controlthread.h"
#include "receivethread.h"
#include "transmitthread.h"

DWORD WINAPI fnControl(LPVOID args)
{
    // parse thread parameters
    ControlArgs* controlArgs = (ControlArgs*) args;
    OutputDebugString("Control thread started\n");

    // create transmit and receive structures
    TransmitArgs transmitArgs;
    ReceiveArgs receiveArgs;

    // initialize transmit structure
    transmitArgs.hRequestStop    = CreateEvent(NULL, TRUE, FALSE, NULL);
    transmitArgs.bStopped        = true;
    transmitArgs.hRequestActive  = CreateEvent(NULL, TRUE, FALSE, NULL);//controlArgs->hRequestTransmit;
    transmitArgs.bActive         = false;
    transmitArgs.bReset          = false;
    transmitArgs.bSYN1           = true;
    transmitArgs.pReceive        = &receiveArgs;
    transmitArgs.pTransmitBuffer = controlArgs->pTransmitBuffer;
    transmitArgs.pHCommPort      = controlArgs->pHCommPort;

    // initialize receive thread structures
    receiveArgs.bRequestStop = false;
    receiveArgs.bStopped     = true;
    receiveArgs.bActive      = false;
    receiveArgs.bRVI         = false;
    receiveArgs.bSYN1        = false;
    receiveArgs.pTransmit    = &transmitArgs;
    receiveArgs.pHCommPort   = controlArgs->pHCommPort;

    // enter main control loop
    while (true)
    {
        // exit look if we're supposed to stop
        if (controlArgs->bRequestStop) {
            OutputDebugString("Control thread stopping\n");
            receiveArgs.bRequestStop = true;
            SetEvent(transmitArgs.hRequestStop);

            if (receiveArgs.bStopped && transmitArgs.bStopped)
            {
                OutputDebugString("Control thread stopped\n");
                controlArgs->bStopped = true;
                return 0;
            }
        }

        // restart any threads that are stopped if the state of both threads are
        // not active
        if (!receiveArgs.bActive && !transmitArgs.bActive)
        {
            if (receiveArgs.bStopped)
            {
                receiveArgs.bRequestStop = false;
                receiveArgs.bStopped = false;
                DWORD threadId;
                CreateThread(NULL, 0, fnReceiveThreadIdle, &receiveArgs, 0,
                        &threadId);
            }
            if (transmitArgs.bStopped)
            {
                ResetEvent(transmitArgs.hRequestStop);
                transmitArgs.bStopped = false;
                DWORD threadId;
                CreateThread(NULL, 0, fnTransmitIdle, &transmitArgs, 0,
                        &threadId);
            }
        }

        // sleep so we don't burn out the core
        Sleep(CONTROL_THREAD_SLEEP_INTERVAL);
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
