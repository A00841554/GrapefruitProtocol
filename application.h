/**
 * declares the constants and functions defined and used in application.cpp
 *
 * @sourceFile application.h
 *
 * @program    Grapefruit.exe
 *
 * @class      Application
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

#include <sstream>
#include "terminal.h"
#include "commport.h"
#include "controlthread.h"

namespace ApplicationConsts
{

    /** modes that the Application class uses to indicate which mode it's in */
    const enum Mode
    {
        CONNECT,
        COMMAND
    };

    /** human-readable string versions of the application modes */
    const std::string ModeNames[] =
    {
        "CONNECT",
        "COMMAND"
    };
}

class Application;

/**
 * interacts with the passed CommPort, and displays information on the passed
 *     Terminal
 *
 * @class        Application
 *
 * @method       Application(HWND, CommPort*, Terminal*)
 * @method       void fnSelectPort(std::string)
 * @method       void fnSetMode(ApplicationConsts::Mode)
 * @method       ApplicationConsts::Mode fnGetMode(void)
 * @method       void fnConfigurePort(void)
 * @method       void fnSend(char*, int)
 * @method       void fnSetRVI(void)
 * @method       void fnHelp(void)
 * @method       bool fnStartControlThread(void)
 * @method       void fnStopControlThread(void)
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
 */
class Application
{
    public:
        Application(HWND, CommPort*, Terminal*);
        void fnSelectPort(std::string);
        void fnSetMode(ApplicationConsts::Mode);
        ApplicationConsts::Mode fnGetMode(void);
        void fnConfigurePort(void);
        void fnSend(char*, int);
        void fnSetRVI(void);
        void fnHelp(void);

    private:
        bool fnStartControlThread(void);
        void fnStopControlThread(void);

        /** structure passed to the control thread */
        ControlArgs controlArgs;

        /** structure passed to the transmit thread */
        TransmitArgs transmitArgs;

        /** structure passed to the receive thread */
        ReceiveArgs receiveArgs;

        /** mode of the Application; it can be CONNECT, or COMMAND */
        ApplicationConsts::Mode mMode;

        /** pointer to communications port used by this instance */
        CommPort* mPtrCommPort;

        /** pointer to terminal used by this instance */
        Terminal* mPtrTerminal;

        /**
         * handle to window so we can get the device context when we want to
         *   draw things
         */
        HWND mHwnd;
};
