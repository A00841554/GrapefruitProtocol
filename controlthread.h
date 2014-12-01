#ifndef _CONTROLTHREAD_H_
#define _CONTROLTHREAD_H_

#include <windows.h>
#include <vector>
#include "typedefs.h"

struct TransmitArgs;
struct ReceiveArgs;

/** structure passed to transmit thread as thread arguments. */
struct TransmitArgs
{
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
    BOOL bActive;       // true if thread is in its "active" state

    BOOL bReset;        // true if state of transmit thread should be "reset"
    BOOL bSYN1;         // true if current data us SYN1

    ReceiveArgs* pReceive;  // pointer to receive thread parameters

    TransmitBuffer* pTransmitBuffer; // pointer to transmit buffer
    HANDLE* pHCommPort;    // reference to serial port
};

/** structure passed to receive thread as thread arguments. */
struct ReceiveArgs
{
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
    BOOL bActive;       // true if thread is in its "active" state

    BOOL bRVI;          // true to send an RVI & switch into transmit mode
    BOOL bSYN1;         // true if current data us SYN1

    TransmitArgs* pTransmit;    // pointer to transmit thread parameters
    HANDLE* pHCommPort;    // reference to serial port
};

/** structure passed to the control thread as thread arguments. */
struct ControlArgs
{
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
    HANDLE hRequestTransmit;// raised to activate the transmit thread to send data

    TransmitBuffer* pTransmitBuffer; // pointer to transmit buffer
    HANDLE* pHCommPort;     // reference to serial port
};

DWORD WINAPI fnControl(LPVOID args);

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

#endif
