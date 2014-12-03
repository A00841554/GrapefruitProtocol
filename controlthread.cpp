#include "controlthread.h"
#include "receivethread.h"
#include "transmitthread.h"

/**
* Creates the control thread and manages what is needed and is being used by it
*
* @sourceFile      controlthread.cpp
*
* @program         Grapefruit.exe
*
* @classes         n/a
*
* @functions
*                
* @date            November 19th, 2014
*
* @revisions
*
* @designer        Eric Tsang
*
* @programmer      Eric Tsang
*
* @notes           none
*/

// Creates one of the control threads that we will be using which is the control thread
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
            // if both the recieve and the transmit thread are stopped
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
            // if stopped, then restart the thread
            if (pReceiveArgs->bStopped)
            {
                ResetEvent(pReceiveArgs->hRequestStop);
                pReceiveArgs->bStopped = false;
                DWORD threadId;
                CreateThread(NULL, 0, fnReceiveThreadIdle, pReceiveArgs, 0,
                        &threadId);
            }
            // if stopped, then restart the thread
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
