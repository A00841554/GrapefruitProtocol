#include <windows.h>
#include <vector>

/** structure passed to transmit thread as thread arguments. */
struct TransmitArgs {
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
    BOOL bActive;       // true if thread is in its "active" state

    BOOL bReset;        // true if state of transmit thread should be "reset"
    BOOL bSYN1;         // true if current data us SYN1

    ReceiveArgs* pReceive;  // pointer to receive thread parameters
    TransmitBuffer* pTransmitBuffer;
};

/** structure passed to receive thread as thread arguments. */
struct ReceiveArgs {
    BOOL bRequestStop;  // true to request the thread to stop
    BOOL bStopped;      // true if thread is stopped; false otherwise
    BOOL bActive;       // true if thread is in its "active" state

    BOOL bRVI;          // true to send an RVI & switch into transmit mode
    BOOL bSYN1;         // true if current data us SYN1

    TransmitArgs* pTransmit;    // pointer to transmit thread parameters
};

typedef struct TransmitArgs TransmitArgs;
typedef struct ReceiveArgs ReceiveArgs;
typedef std::vector<char> TransmitBuffer;

