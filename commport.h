
/**
 * declares a CommPort class, related constants, and return codes. definitions
 *     of the CommPort class's methods are in commport.cpp
 *
 * @sourceFile   commport.h
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      CommPort
 *
 * @functions    none
 *
 * @date         2014-09-26
 *
 * @revisions
 *
 * 2014-09-27 - Eric Tsang - added a destructor to the CommPort class so that it
 *     will clean itself up before being deleted; now it closes the serial port
 *     before being deleted.
 *
 * 2014-09-08 - Eric Tsang - added a handle to a read thread as a instance
 *     variable for the CommPort class. this way, this object can keep track of
 *     the read thread, and start or stop it whenever it likes
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * declares a CommPort class.
 *
 * defines constants used to initialize the underlying serial port.
 *
 * defines return codes used in many CommPort methods to indicate the outcome of
 *     the operation
 */
#include <windows.h>
#include <sstream>




/////////////////////////////////////
// Open serial port default values //
/////////////////////////////////////

/** default dwDesiredAccess value for the CreateFile function */
const DWORD DW_DEF_DESIRED_ACCESS = GENERIC_READ | GENERIC_WRITE;

/** default dwShareMode value for the CreateFile function */
const DWORD DW_DEF_SHARE_MODE = 0;

/** default lpSecurityAttributes value for the CreateFile function */
const LPSECURITY_ATTRIBUTES LP_DEF_SECURITY_ATTRS = NULL;

/** default dwCreationDistribution value for the CreateFile function */
const DWORD DW_DEF_CREATION_DISTRIBUTION = OPEN_EXISTING;

/** default dwFlagsAndAttributes value for the CreateFile function */
const DWORD DW_DEF_FLAGS_AND_ATTRS = NULL;
//const DWORD DW_DEF_FLAGS_AND_ATTRS = FILE_FLAG_OVERLAPPED;

/** default hTemplateFile value for the CreateFile function */
const HANDLE H_DEF_TEMPLATE_FILE = NULL;




/////////////////////////////////////////////
// Serial port send/receive timeout values //
/////////////////////////////////////////////

/** time in milliseconds to wait for delayed reading in asynchronous IO */
const int READ_TIMEOUT = 5000;

/** default value for readIntervalTimeout of serial port timeouts */
const int DEF_READ_INTERVAL_TIMEOUT = 1;

/** default value for readTotalTimeoutConstant of serial port timeouts */
const int DEF_READ_TOTAL_TIMEOUT_CONSTANT = 1;

/** default value for readTotalTimeoutMultiplier of serial port timeouts */
const int DEF_READ_TOTAL_TIMEOUT_MULTIPLIER = 1;

/** default value for writeTotalTimeoutConstant of serial port timeouts */
const int DEF_WRITE_TOTAL_TIMEOUT_CONSTANT = 1;

/** default value for writeTotalTimeoutMultiplier of serial port timeouts */
const int DEF_WRITE_TOTAL_TIMEOUT_MULTIPLIER = 1;




//////////////////
// Return codes //
//////////////////

/** return code when an operation succeeds */
const int SUCCESS = 200;

/** return code when an operation fails */
const int FAIL = 201;

/**
 * return code indicating that the operation is not allowed because of the
 *   object's current state
 */
const int INVALID_OPERATION_FOR_STATE = 204;




//////////////////////////
// Forward declarations //
//////////////////////////

/** serial port object */
class CommPort;

/** function that's run on the read thread */
DWORD WINAPI fnReadThread(LPVOID);

/**
 * provides interface methods for serial ports, and blocks nonsensical sequences
 *   of function calls from corrupting the serial port.
 *
 * @class        CommPort
 *
 * @methods      CommPort::CommPort(std::string portName,
 *                       void (*onReadCallback)(char))
 *               CommPort::~CommPort(void)
 *               int CommPort::fnSetPortName(std::string portName)
 *               std::string CommPort::fnGetPortName(void)
 *               Status fnGetPortStatus(void)
 *               void fnStartReadThread(void);
 *               void fnEndReadThread(void);
 *               bool fnIsReadThread(DWORD threadId);
 *               int CommPort::fnConfigurePort(HWND hwnd)
 *               int CommPort::fnOpen(void)
 *               int CommPort::fnClose(void)
 *               int CommPort::fnSend(char c)
 *               int CommPort::fnRead(void)
 *               DWORD WINAPI fnReadThread(LPVOID threadParams)
 *
 * @date         2014-09-26
 *
 * @revisions
 *
 * 2014-09-27 - Eric Tsang - added a destructor to the CommPort class so that it
 *     will clean itself up before being deleted; now it closes the serial port
 *     before being deleted.
 *
 * 2014-09-28 - Eric Tsang - added a dword for the read thread's id as an
 *     instance variable. this way, instances can keep track of the read thread,
 *     and start or stop it whenever it likes.
 *
 * 2014-09-28 - Eric Tsang - added 2 methods; fnStartReadThread,
 *     fnEndReadThread, and fnIsReadThread
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * manages a serial port.
 *
 * interface functions of this object can be called at any time to interact with
 *     the serial port, but this object will prevent any unreasonable sequence
 *     of actions affect the underlying serial port.
 *
 * return codes are returned by most of the interface functions indicating the
 *     result of the operation and is intended to be used to update user
 *     interfaces as needed.
 */
class CommPort
{
    public:
        /** status codes for a serial port */
        enum Status
        {
            OPENED,
            CLOSED
        };
        CommPort(std::string, void(char));
        CommPort::~CommPort(void);
        std::string fnGetPortName(void);
        Status fnGetPortStatus(void);
        void fnStartReadThread(void);
        void fnEndReadThread(void);
        bool CommPort::fnIsReadThread(DWORD);
        int fnSetPortName(std::string);
        int fnConfigurePort(HWND);
        int fnOpen(void);
        int fnClose(void);
        int fnSend(char c);
        int fnRead(void);

    private:
        /** status of serial port; it can be OPENED, or CLLOSED */
        Status mStatus;

        /** handle to serial port */
        HANDLE mHComm;

        /** ID of the read thread */
        DWORD mReadThreadId;

        /** name of the serial port */
        std::string mPortName;

        /** serial port's internal DCB structure */
        DCB mDcb;

        /** serial port's COMMPROP structure  */
        COMMPROP mCommProp;

        /** used to configure the serial port */
        COMMCONFIG mCommConfig;

        /**
         * structure containing timeout variables for reading and writing to and
         *   from the comm port
         */
        COMMTIMEOUTS mCommTimeouts;

        /**
         * serial port's OVERLAPPED structure used by OS for asynchronous IO
         *   purposes
         */
        OVERLAPPED mReadOverlapped;

        /**
         * serial port's OVERLAPPED structure used by OS for asynchronous IO
         *   purposes
         */
        OVERLAPPED mWriteOverlapped;

        /** true if a read is in progress; false otherwise */
        bool mReadInProgress;

        /** buffer where characters are read into from the serial port */
        char mInBuffer;

        /** number of bytes read from serial port in a read operation */
        DWORD mBytesRead;

        /**
         * pointer to callback function that is invoked when something is
         *   received from the serial port
         */
        void (*mOnRead)(char);
};
