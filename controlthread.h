#ifndef _CONTROLTHREAD_H_
#define _CONTROLTHREAD_H_

#include <windows.h>
#include <vector>
<<<<<<< HEAD

struct TransmitArgs;
struct ReceiveArgs;
typedef std::vector<char> TransmitBuffer;
=======
>>>>>>> joel

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
<<<<<<< HEAD
<<<<<<< HEAD
=======

    HANDLE hCommPort;   // handle to the serial port
>>>>>>> 5dd568b2fe65d7af8a117413bed6b3e04e3ebabf
};
=======
};

typedef struct TransmitArgs TransmitArgs;
typedef struct ReceiveArgs ReceiveArgs;
typedef std::vector<char> TransmitBuffer;

<<<<<<< HEAD
>>>>>>> joel
=======
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
>>>>>>> 5dd568b2fe65d7af8a117413bed6b3e04e3ebabf
