/**
 * implementation of the functions declared in application.h
 *
 * @sourceFile application.cpp
 *
 * @program    Grapefruit.exe
 *
 * @function   Application::Application(HWND, CommPort*, Terminal*)
 * @function   void Application::fnSelectPort(std::string)
 * @function   void Application::fnSetMode(ApplicationConsts::Mode)
 * @function   ApplicationConsts::Mode Application::fnGetMode(void)
 * @function   void Application::fnConfigurePort(void)
 * @function   void Application::fnSend(char*, int)
 * @function   void Application::fnHelp(void)
 * @function   bool Application::fnStartControlThread(void)
 * @function   void Application::fnStopControlThread(void)
 * @function   void Application::fnSetRVI(void)
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 */

#include "application.h"




/////////////////
// Constructor //
/////////////////

/**
 * instantiates an application object
 *
 * @class      Application
 *
 * @method     Application
 *
 * @date       2014-12-03
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       initializes instance variables as necessary
 *
 * @signature  Application::Application(HWND, CommPort*, Terminal*)
 *
 * @param      hwnd handle to the window
 * @param      commPort handle to the serial port that the application uses
 * @param      terminal handle to the terminal used to print the status of the
 *   application to
 */
Application::Application(HWND hwnd, CommPort* commPort, Terminal* terminal)
{

    // initialize private members from function parameters
    mPtrCommPort = commPort;
    mPtrTerminal = terminal;
    mHwnd = hwnd;

    // initialize control argument structure
    controlArgs.bStopped = true;
}




/////////////////////////
// Interface functions //
/////////////////////////

/**
 * changes the serial port used by the application; prints a message to the
 *   passed terminal indicating the result of the operation.
 *
 * @class      Application
 *
 * @method     fnSelectPort
 *
 * @date       2014-12-03
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnSelectPort(std::string)
 *
 * @param      newPortName name of the port to try to use next
 */
void Application::fnSelectPort(std::string newPortName)
{

    // create a string stream that we will use to build a string, and print to
    // the terminal
    std::stringstream mStringStream;

    // validate application mode before continuing
    ApplicationConsts::Mode validMode = ApplicationConsts::Mode::COMMAND;
    if (mMode != validMode)
    {
        mStringStream << "Must be in ";
        mStringStream << ApplicationConsts::ModeNames[validMode];
        mStringStream << " mode to change ports";
        mStringStream << "\r\n";

    }
    else
    {

        // close the current port, change ports, and open the specified port
        (*mPtrCommPort).fnClose();
        (*mPtrCommPort).fnSetPortName(newPortName);
        switch ((*mPtrCommPort).fnOpen())
        {

            case FAIL:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << " does not exist" << "\r\n";
                break;

            case SUCCESS:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << " selected" << "\r\n";
                break;

            case INVALID_OPERATION_FOR_STATE:
                mStringStream << "Error changing ports; using serial port: ";
                mStringStream << (*mPtrCommPort).fnGetPortName() << "\r\n";
                break;
        }
    }

    // print whatever was in the string stream to the terminal
    (*mPtrTerminal).fnPrint(mStringStream.str());
}

/**
 * sets the mode of the application to the passed mode; prints the new mode to
 *   the terminal passed in through the constructor.
 *
 * @class      Application
 *
 * @method     fnSetMode
 *
 * @date       2014-12-03
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note
 *
 * performs cleanup code needed to leave the current mode, and setup
 *   setup code to go into the new mode, then sets the current mode of the
 *   application to the new mode
 *
 * @signature  void Application::fnSetMode(ApplicationConsts::Mode)
 *
 * @param      newMode number indicating the new mode that the application
 *   should switch to.
 */
void Application::fnSetMode(ApplicationConsts::Mode newMode)
{
    std::stringstream mStringStream;
    bool failedToChangeModes = false;

    // change to the new mode
    if (mMode != newMode)
    {

        // do whatever you need to do, before changing modes
        switch (mMode)
        {
            case ApplicationConsts::Mode::COMMAND:
                // nothing to do
                break;
            case ApplicationConsts::Mode::CONNECT:
                fnStopControlThread();
                break;
        }

        // do whatever you need to do, after changing modes
        switch (newMode)
        {
            case ApplicationConsts::Mode::COMMAND:
                // nothing to do
                break;
            case ApplicationConsts::Mode::CONNECT:
                failedToChangeModes = !fnStartControlThread();
                break;
        }

        if (!failedToChangeModes)
        {
            mMode = newMode;
        }
        else
        {
            mStringStream << "Failed to change modes; ";
        }
        mStringStream << "Now in ";
        mStringStream << ApplicationConsts::ModeNames[mMode];
        mStringStream << " mode" << "\r\n";
    }
    else
    {
        mStringStream << "Already in " << ApplicationConsts::ModeNames[mMode];
        mStringStream << " mode" << "\r\n";
    }
    (*mPtrTerminal).fnPrint(mStringStream.str());

}

/**
 * returns the current mode of the application
 *
 * @class      Application
 *
 * @method     fnGetMode
 *
 * @date       2014-12-03
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  ApplicationConsts::Mode Application::fnGetMode(void)
 *
 * @return     current mode of the application
 */
ApplicationConsts::Mode Application::fnGetMode(void)
{
    return mMode;
}

/**
 * displays a window that a user can use to modify the properties of the serial
 *   port; prints the result of the configuration to the Terminal passed during
 *   construction.
 *
 * @class      Application
 *
 * @method     fnConfigurePort
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnConfigurePort(void)
 */
void Application::fnConfigurePort(void)
{

    // create a string stream that we will use to build a string, and print to
    // the terminal
    std::stringstream mStringStream;

    // validate application mode before continuing
    ApplicationConsts::Mode validMode = ApplicationConsts::Mode::COMMAND;
    if (mMode != validMode)
    {
        mStringStream << "Must be in ";
        mStringStream << ApplicationConsts::ModeNames[validMode];
        mStringStream << " mode to configure the port";
        mStringStream << "\r\n";

    }
    else
    {

        // go on to configure the port
        switch ((*mPtrCommPort).fnConfigurePort(mHwnd))
        {

            case SUCCESS:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << " configured";
                mStringStream << "\r\n";
                break;

            case FAIL:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << " configuration failed";
                mStringStream << "\r\n";
                break;

            case INVALID_OPERATION_FOR_STATE:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << " must be open to be configured";
                mStringStream << "\r\n";
                break;
        }
    }

    // print whatever was in the string stream to the terminal
    (*mPtrTerminal).fnPrint(mStringStream.str());
}

/**
 * sends the passed character array to the out through the communications port
 *
 * @class      Application
 *
 * @method     fnSend
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnSend(char*, int)
 *
 * @param      pBuffer pointer to the first character in the character array
 * @param      nCharsToSend length of the character array
 */
void Application::fnSend(char* pBuffer, int nCharsToSend)
{

    // create a string stream that we will use to build a string, and print to
    // the terminal
    std::stringstream mStringStream;

    // validate application mode before continuing
    ApplicationConsts::Mode validMode = ApplicationConsts::Mode::CONNECT;
    if (mMode != validMode)
    {
        mStringStream << "Must be in ";
        mStringStream << ApplicationConsts::ModeNames[validMode];
        mStringStream << " mode to send data";
        mStringStream << "\r\n";

    }
    else
    {

        // send the character out through the port
        switch ((*mPtrCommPort).fnSend(pBuffer, nCharsToSend))
        {

            case SUCCESS:
                SetEvent(controlArgs.pTransmit->hRequestActive);
                break;

            case FAIL:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << ": failed to send for unknown reason" << "\r\n";
                break;

            case INVALID_OPERATION_FOR_STATE:
                mStringStream << (*mPtrCommPort).fnGetPortName();
                mStringStream << ": cannot send while port is closed" << "\r\n";
                break;
        }
    }

    // print whatever was in the string stream to the terminal
    (*mPtrTerminal).fnPrint(mStringStream.str());
}

/**
 * prints a helpful message for the user about how to use the program
 *
 * @class      Application
 *
 * @method     fnHelp
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnHelp(void)
 */
void Application::fnHelp(void)
{

    // create & print things to a string stream
    std::stringstream mStringStream;
    mStringStream << " Manual:    > Select Port:        Select the Comm Port that is going to be used.\r\n";
    mStringStream << "                  > Configure Port:  Configures the Current Comm Port being used.\r\n";
    mStringStream << "                  > Clear Screen:    Clears the current status screen.\r\n";
    mStringStream << "                  > Change Mode:   Selects the mode in which you are going to operate.\r\n";

    (*mPtrTerminal).fnClearScreen();
    (*mPtrTerminal).fnPrint(mStringStream.str());
}

/**
 * starts the control thread that manages the transmit and receive threads
 *
 * @class      Application
 *
 * @method     fnStartControlThread
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  bool Application::fnStartControlThread(void)
 *
 * @return     true if the ControlThread is running; false otherwise
 */
bool Application::fnStartControlThread(void)
{
    if (controlArgs.bStopped) {

        // initialize control thread structure
        controlArgs.bRequestStop     = false;
        controlArgs.bStopped         = false;
        controlArgs.pTransmitBuffer  = mPtrCommPort->fnGetTransmitBuffer();
        controlArgs.hCommPort        = mPtrCommPort->fnGetCommHandle();
        controlArgs.pTransmit        = &transmitArgs;
        controlArgs.pReceive         = &receiveArgs;

        // initialize transmit thread structure
        transmitArgs.hRequestStop    = CreateEvent(NULL, TRUE, FALSE, NULL);
        transmitArgs.bStopped        = true;
        transmitArgs.hRequestActive  = CreateEvent(NULL, TRUE, FALSE, NULL);
        transmitArgs.bActive         = false;
        transmitArgs.bReset          = false;
        transmitArgs.bSYN1           = true;
        transmitArgs.pReceive        = &receiveArgs;
        transmitArgs.pTransmitBuffer = controlArgs.pTransmitBuffer;
        transmitArgs.hCommPort       = controlArgs.hCommPort;

        // initialize receive thread structures
        receiveArgs.hRequestStop     = CreateEvent(NULL, TRUE, FALSE, NULL);
        receiveArgs.bStopped         = true;
        receiveArgs.bActive          = false;
        receiveArgs.bRVI             = false;
        receiveArgs.bSYN1            = false;
        receiveArgs.pTransmit        = &transmitArgs;
        receiveArgs.hCommPort        = controlArgs.hCommPort;

        // start the control thread
        DWORD threadId;
        CreateThread(NULL, 0, fnControl, &controlArgs, 0, &threadId);
    }

    return !controlArgs.bStopped;
}

/**
 * sends a message to the control thread to ask it to stop
 *
 * @class      Application
 *
 * @method     fnStopControlThread
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnStopControlThread(void)
 */
void Application::fnStopControlThread(void)
{
    controlArgs.bRequestStop = true;
}

/**
 * sets the RVI flag to true so that communication reverses, and we will become
 *   the transmitter
 *
 * @class      Application
 *
 * @method     fnSetRVI
 *
 * @date       2014-12-03
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  void Application::fnSetRVI(void)
 */
void Application::fnSetRVI(void)
{
    if (mMode == ApplicationConsts::Mode::CONNECT)
        controlArgs.pReceive->bRVI = true;
}
