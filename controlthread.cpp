#include "controlthread.h"
#include "receivethread.h"
#include "transmitthread.h"

DWORD WINAPI fnControl(LPVOID args)
{

    OutputDebugString("Control thread started\n");
    
    // parse thread parameters
    ControlArgs* controlArgs = (ControlArgs*) args;
    TransmitArgs* pTransmitArgs = controlArgs->pTransmit;
    ReceiveArgs* pReceiveArgs = controlArgs->pReceive;

    // enter main control loop
    while (true)
    {
        // exit look if we're supposed to stop
        if (controlArgs->bRequestStop) {
            OutputDebugString("Control thread stopping\n");
            SetEvent(pReceiveArgs->hRequestStop);
            SetEvent(pTransmitArgs->hRequestStop);

            if (pReceiveArgs->bStopped && pTransmitArgs->bStopped)
            {
                OutputDebugString("Control thread stopped\n");
                controlArgs->bStopped = true;
                return 0;
            }
        }

        // restart any threads that are stopped if the state of both threads are
        // not active
        else if (!pReceiveArgs->bActive && !pTransmitArgs->bActive)
        {
            if (pReceiveArgs->bStopped)
            {
                ResetEvent(pReceiveArgs->hRequestStop);
                pReceiveArgs->bStopped = false;
                DWORD threadId;
                CreateThread(NULL, 0, fnReceiveThreadIdle, pReceiveArgs, 0,
                        &threadId);
            }
            if (pTransmitArgs->bStopped)
            {
                ResetEvent(pTransmitArgs->hRequestStop);
                pTransmitArgs->bStopped = false;
                DWORD threadId;
                CreateThread(NULL, 0, fnTransmitIdle, pTransmitArgs, 0,
                        &threadId);
            }
        }

        // sleep so we don't burn out the core
        Sleep(SHORT_SLEEP);
    }
}

/*
#include <iostream>
int main(void) {
    ReceiveArgs a;
    a.bRequestStop = false;
    a.bStopped = false;
    a.bActive = false;
    std::cout << a.bActive << "\r\n";
    return 0;
}
*/
