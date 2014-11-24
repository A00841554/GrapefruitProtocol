
/**
 * declares an Application class. definitions of its methods are in
 *     application.cpp
 *
 * @sourceFile   application.h
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      Application
 *
 * @functions
 *
 * 2014-09-07 - Eric Tsang - added modes to the Application class; Application
 *     instances now be in specific modes. different functionalities are
 *     available depending on the modes that they are in
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
 * @methods      Application::Application(HWND hwnd, CommPort* commPort,
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
 * @date         2014-09-25
 *
 * @revisions
 *
 * 2014-09-27 - Eric Tsang - replaced the fnOpenPort and fnClosePort functions
 *     with the fnSetMode and fbGetMode methods to conform more to the
 *     requirements of this assignment. added a mode variable (mMode) that holds
 *     the current mode of the Application instance; it can be
 *     ApplicationConsts::Mode::CONNECT or ApplicationConsts::Mode::COMMAND
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @notes
 *
 * provides interface methods to interact with the passed CommPort, and displays
 *     information to the user through the passed Terminal
 */
class Application
{
    public:
        Application(HWND, CommPort*, Terminal*);
        void fnSelectPort(std::string);
        void fnSetMode(ApplicationConsts::Mode);
        ApplicationConsts::Mode fnGetMode(void);
        void fnConfigurePort(void);
        void fnSend(char);
        void fnOnReceive(char);
        void fnHelp(void);

    private:
        bool fnStartControlThread(void);
        void fnStopControlThread(void);

        /** structure passed to the control thread */
        ControlArgs controlArgs;

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
