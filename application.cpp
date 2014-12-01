
/**
 * definitions of the Application class's functions declared in application.h
 *
 * @sourceFile   application.cpp
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      Application
 *
 * @functions    Application::Application(HWND hwnd, CommPort* commPort,
 *                       Terminal* terminal)
 *               void Application::fnSelectPort(std::string newPortName)
 *               void fnSetMode(ApplicationConsts::Mode newMode)
 *               ApplicationConsts::Mode newMode fnGetMode(void)
 *               void Application::fnOpenPort(void)     REMOVED
 *               void Application::fnClosePort(void)    REMOVED
 *               void Application::fnConfigurePort(void)
 *               void Application::fnSend(char c)
 *               void Application::fnHelp(void)
 *               void Application::fnOnReceive(char c)
 *
 * @date         2014-09-26
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 */
#include "application.h"




/////////////////
// Constructor //
/////////////////

/**
 * instantiates an Application object
 *
 * @class        Application
 *
 * @method       Application
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
 * instantiates an Application object, and saves pointers to passed CommPort and
 *     Terminal objects.
 *
 * @signature    Application::Application(HWND hwnd, CommPort* commPort,
 *     Terminal* terminal)
 *
 * @param        hwnd   handle to the window that this Application object is
 *     associated with, so it can show dialogs as needed
 * @param        commPort   pointer to a CommPort object that this Application
 *     uses, and provices functions for, and displays information about
 * @param        terminal   pointer to a Terminal object used to display
 *     information to the user
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
 * tries to sets the name of the CommPort object; displays a message on the
 *     Terminal about the outcome of the operation
 *
 * @class        Application
 *
 * @method       fnSelectPort
 *
 * @date         2014-09-26
 *
 * @revisions
 *
 * 2014-09-27 - automatically closes the current port, before changing the name,
 *     then automatically tries to open the new port.
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    void Application::fnSelectPort(std::string newPortName)
 *
 * @param        newPortName   name of the port that we want the application to
 *     use
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
 * sets the current mode that the application is in
 *
 * @class        Application
 *
 * @method       fnSetMode
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
 * sets the current mode that the Application instance is in. in different
 *     modes, different functions are available, and others are blocked. valid
 *     Application modes are in the ApplicationConsts::Mode namespace
 *
 * @signature    void Application::fnSetMode(ApplicationConsts::Mode newMode)
 *
 * @param        newMode   the new mode of the Application instance
 */
void Application::fnSetMode(ApplicationConsts::Mode newMode)
{
    std::stringstream mStringStream;
    bool failedToChangeModes = false;

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

    // change to the new mode
    if (mMode != newMode)
    {
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
 * returns the Application instance's current mode
 *
 * @class        Application
 *
 * @method       fnGetMode
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
 * @signature    ApplicationConsts::Mode Application::fnGetMode(void)
 *
 * @return       current mode of the Application instance
 */
ApplicationConsts::Mode Application::fnGetMode(void)
{
    return mMode;
}

/**
 * Tries to open a dialog that can be used to configure common settings of the
 *   CommPort; prints a message about the result of the operation to the
 *   Terminal
 *
 * @class        Application
 *
 * @method       fnConfigurePort
 *
 * @date         2014-09-25
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    void Application::fnConfigurePort(void)
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
 * tries to send the passed character (c) out the CommPort; prints a message to
 *   the Terminal if the operation fails. nothing is printed to the Terminal if
 *   the operation succeeds.
 *
 * @class        Application
 *
 * @method       fnSend
 *
 * @date         2014-09-25
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    void Application::fnSend(char c)
 *
 * @param        c   character to send out the CommPort
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
                SetEvent(controlArgs.hRequestTransmit);
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
 * prints a nice helpful help message to the Terminal
 *
 * @class        Application
 *
 * @method       fnHelp
 *
 * @date         2014-09-25
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes        none
 *
 * @signature    void Application::fnHelp(void)
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
 * handles a received character
 *
 * @class        Application
 *
 * @method       fnOnReceive
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
 * handles a received character; when a character is received through the serial
 *     port, this callback function is invoked. the passed character (c) is a
 *     character received from the CommPort. the passed character (c) is printed
 *     to the Terminal...unless its a backspace.
 *
 * @signature    void Application::fnOnReceive(char c)
 *
 * @param        c   character received from the CommPort
 */
void Application::fnOnReceive(char c)
{

    // create a string stream that we will use to build a string, and print to
    // the terminal
    std::stringstream mStringStream;

    // validate application mode before continuing
    if (mMode == ApplicationConsts::Mode::CONNECT)
    {

        // do something to the Terminal depending on the received character
        std::stringstream mStringStream;
        switch (c)
        {

            // backspace key; remove character
            case 8:
            {
                //(*mPtrTerminal).fnBackspace();
                break;
            }

            // return key; go to the next line
            case 13:
                mStringStream << "\n";
                break;

            // everything else; append character
            default:
                mStringStream << c;
                break;
        }
        (*mPtrTerminal).fnPrint(mStringStream.str());
    }
}

/**
 * starts the control thread
 *
 * @class        Application
 *
 * @method       fnStartControlThread
 *
 * @date         2014-11-23
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * starts the control thread. returns true if the control thread was started;
 *   false otherwise.
 *
 * @signature    bool Application::fnStartControlThread(void)
 */
bool Application::fnStartControlThread(void)
{
    if (controlArgs.bStopped) {
        controlArgs.bRequestStop = false;
        controlArgs.bStopped = false;
        controlArgs.hRequestTransmit = CreateEvent(NULL, TRUE, FALSE, NULL);
        controlArgs.pTransmitBuffer = mPtrCommPort->fnGetTransmitBuffer();
        controlArgs.pHCommPort = mPtrCommPort->fnGetCommHandle();

        DWORD threadId;

        CreateThread(NULL, 0, fnControl, &controlArgs, 0, &threadId);
    }

    return !controlArgs.bStopped;
}

void Application::fnStopControlThread(void)
{
    controlArgs.bRequestStop = true;
}
