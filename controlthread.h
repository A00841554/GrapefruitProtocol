#ifndef _CONTROLTHREAD_H_
#define _CONTROLTHREAD_H_

#include <windows.h>
#include <vector>

struct TransmitArgs;
struct ReceiveArgs;
typedef std::vector<char> TransmitBuffer;

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

    HANDLE hCommPort;   // handle to the serial port
};

/** structure passed to the control thread as thread arguments. */
struct ControlArgs
{
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
};

DWORD WINAPI fnControl(LPVOID args);

const int CONTROL_THREAD_SLEEP_INTERVAL = 1000;

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

#endif