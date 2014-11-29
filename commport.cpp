
/**
 * method definitions of the CommPort declarations in commport.h
 *
 * @sourceFile   commport.cpp
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      CommPort
 *
 * @functions    CommPort::CommPort(std::string portName,
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
 * 2014-09-28 - Eric Tsang - making use of the read thread id instance variable
 *
 * 2014-09-27 - Eric Tsang - added a destructor to the CommPort class so that it
 *     will clean itself up before being deleted; now it closes the serial port
 *     before being deleted.
 *
 * 2014-09-28 - Eric Tsang - added 2 methods; fnStartReadThread and
 *     fnEndReadThread
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 */
#include "commport.h"




/////////////////
// Constructor //
/////////////////

/**
 * Instantiates a comm port object
 *
 * @class        CommPort
 *
 * @method       CommPort
 *
 * @date         2014-09-26
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * instantiates a CommPort object, which manages the serial port with the passed
 *     name (portName)
 *
 * initializes some instance variables used for the internal operation of the
 *     instance, and some structures to be used by the OS
 *
 * @signature    CommPort::CommPort(std::string portName, void
 *     (*onReadCallback)(char))
 *
 * @param        portName   initial name of the port that this instance is to
 *     manage
 * @param        onReadCallback   pointer to function that will be invoked when
 *     this CommPort receives a character
 */
CommPort::CommPort(std::string portName)
{

    // initialize variables
    mStatus = Status::CLOSED;

    // initialize variables from function parameters
    mPortName = portName;

    // set up COMMTIMEOUTS structure
    memset(&mCommTimeouts, 15, sizeof(COMMTIMEOUTS));

    // set up COMMCONFIG structure
    memset(&mCommConfig, 0, sizeof(COMMCONFIG));
    mCommConfig.dwSize = sizeof(COMMCONFIG);
    mCommConfig.wVersion = 0x100;
}




////////////////
// Destructor //
////////////////

/**
 * closes the underlying serial port before it goes on disappear from memory
 *
 * @class        CommPort
 *
 * @method       ~CommPort
 *
 * @date         2014-09-27
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    c CommPort::~CommPort(void)
 */
CommPort::~CommPort(void)
{

    // close the port before dying
    fnClose();
}




/////////////////////////
// Interface functions //
/////////////////////////

/**
 * sets the name of the port that this instance manages
 *
 * @class        CommPort
 *
 * @method       fnSetPortName
 *
 * @date         2014-09-26
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * return codes are defined in commport.h.
 *
 * INVALID_OPERATION_FOR_STATE is returned if the CommPort is in the OPENED
 *     state. nothing will happen to the underlying serial port
 *
 * SUCCESS will be returned when the name is successfully changed to the passed
 *     name (portName)
 *
 * @signature    int CommPort::fnSetPortName(std::string portName)
 *
 * @param        portName   new name of the port
 *
 * @return       a return code indicating the result of the operation: SUCCESS,
 *     INVALID_OPERATION_FOR_STATE
 */
int CommPort::fnSetPortName(std::string portName)
{

    // abort setting name of serial port if the port is already opened.
    if (mStatus == Status::OPENED)
        return INVALID_OPERATION_FOR_STATE;

    // set name of serial port
    mPortName = portName;
    return SUCCESS;
}

/**
 * returns the status of the CommPort instance
 *
 * @class        CommPort
 *
 * @method       fnGetPortStatus
 *
 * @date         2014-09-27
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * CommPort::Status::CLOSED is returned when the CommPort instance is closed.
 *
 * CommPort::Status::OPENED is returned when the CommPort instance is open
 *
 * @signature    Status fnGetPortStatus(void)
 *
 * @return       returns the status of the CommPort: CommPort::Status::OPENED,
 *     CommPort::Status::CLOSED
 */
CommPort::Status CommPort::fnGetPortStatus(void)
{
    return mStatus;
}

/**
 * returns the name of port that this CommPort instance is managing
 *
 * @class        CommPort
 *
 * @method       fnGetPortName
 *
 * @date         2014-09-24
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    std::string CommPort::fnGetPortName(void)
 *
 * @return       string containing name of port that instance is managing
 */
std::string CommPort::fnGetPortName(void)
{
    return mPortName;
}

/**
 * opens a dialog box used to configure the serial port.
 *
 * @class        CommPort
 *
 * @method       fnConfigurePort
 *
 * @date         2014-09-26
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * INVALID_OPERATION_FOR_STATE is returned CommPort instance is CLOSED; the port
 *     cannot be configured if it is not OPENED
 *
 * FAIL is returned if the configuration fails for some reason, or if the user
 *     cancels it
 *
 * SUCCESS will be returned if the user successfully configures the dialog box
 *
 * @signature    int CommPort::fnConfigurePort(HWND hwnd)
 *
 * @param        hwnd   handle to parent window of dialog that will pop up
 *
 * @return       return code indicating the result of the operation:
 *     INVALID_OPERATION_FOR_STATE, SUCCESS, FAIL
 */
int CommPort::fnConfigurePort(HWND hwnd)
{

    // if the port is closed, bail out; cannot configure a closed port
    if (mStatus == Status::CLOSED)
    {
        return INVALID_OPERATION_FOR_STATE;
    }

    // configure the port...
    GetCommConfig(mHComm, &mCommConfig, &mCommConfig.dwSize);
    if (CommConfigDialogA(mPortName.c_str(), hwnd, &mCommConfig))
    {
        if (SetCommConfig(mHComm, &mCommConfig, mCommConfig.dwSize))
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    }
}

/**
 * opens the serial port that this CommPort instance manages
 *
 * @class        CommPort
 *
 * @method       fnOpen
 *
 * @date         2014-09-26
 *
 * @revisions
 *
 * 2014-09-28 - Eric Tsang - removed starting of the read thread from this
 *     method
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * opens the serial port, sets up associated structures, and starts the read
 *     thread for the comm port that this instance manages
 *
 * FAIL is returned if the current port name of the instance doesn't exist on
 *     the computer
 *
 * INVALID_OPERATION_FOR_STATE is returned if the CommPort instance is already
 *     in a OPENED state.
 *
 * SUCCESS is returned if the port exists, and the CommPort instance is
 *     currently closed
 *
 * @signature    int CommPort::fnOpen(void)
 *
 * @return       return code indicating the result of the operation:
 *     INVALID_OPERATION_FOR_STATE, SUCCESS, FAIL
 */
int CommPort::fnOpen(void)
{

    // bail out if the port is already open; you can't open the port if it's
    // already open
    if (mStatus == Status::OPENED)
    {
        return INVALID_OPERATION_FOR_STATE;
    }

    // try to open the port
    mHComm = CreateFile(
            mPortName.c_str(),              // lpFileName
            DW_DEF_DESIRED_ACCESS,          // dwDesiredAccess
            DW_DEF_SHARE_MODE,              // dwShareMode
            LP_DEF_SECURITY_ATTRS,          // lpSecurityAttributes
            DW_DEF_CREATION_DISTRIBUTION,   // dwCreationDistribution
            DW_DEF_FLAGS_AND_ATTRS,         // dwFlagsAndAttributes
            H_DEF_TEMPLATE_FILE );          // hTemplateFile

    // set status to reflect the status of the serial port as it may not have
    // been opened, and start the port listener
    if (mHComm != INVALID_HANDLE_VALUE
            && GetCommProperties(mHComm, &mCommProp)
            && GetCommState(mHComm, &mDcb)
            && BuildCommDCB("96,N,8,1", &mDcb)
            && SetCommState(mHComm, &mDcb)
            && SetCommMask(mHComm, EV_RXCHAR)
            && SetCommTimeouts(mHComm, &mCommTimeouts))
    {

        // set status to reflect status of serial port
        mStatus = Status::OPENED;

        // return...
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

/**
 * closes the serial port
 *
 * @class        CommPort
 *
 * @method       fnClose
 *
 * @date         2014-09-25
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * closes the CommPort instance
 *
 * INVALID_OPERATION_FOR_STATE is returned when the status of the CommPort
 *     instance is already CLOSED.
 *
 * FAIL is returned when the CommPort fails to close for some random reason.
 *
 * SUCCESS is returned when the CommPort instance is closed successfully.
 *
 * @signature    int CommPort::fnClose(void)
 *
 * @return       return code which indicates the result of the operation:
 *     INVALID_OPERATION_FOR_STATE, SUCCESS, FAIL
 */
int CommPort::fnClose(void)
{

    // verify state; cannot close the port if it's already closed
    if (mStatus == Status::CLOSED)
        return INVALID_OPERATION_FOR_STATE;

    // try to close the port
    if (CloseHandle(mHComm))
    {
        mStatus = Status::CLOSED;    // update status
        return SUCCESS;              // return...
    }
    else
    {
        return FAIL;
    }
}

TransmitBuffer* CommPort::fnGetTransmitBuffer(void)
{
    return &transmitBuffer;
}

HANDLE* CommPort::fnGetCommHandle(void)
{
    return &mHComm;
}

int CommPort::fnSend(char* pBuffer, DWORD nBytesToSend)
{
    // verify state; cannot close the port if it's already closed
    if (mStatus == Status::CLOSED)
        return INVALID_OPERATION_FOR_STATE;

	for (int i = 0; i < nBytesToSend; ++i)
	{
        transmitBuffer.push_back(pBuffer[i]);
	}
}