
/**
 * windowed, terminal-like program that that lets one use a serial port to send,
 *     and receive data.
 *
 * @sourceFile   main.cpp
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      none
 *
 * @functions    int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 *                       LPSTR lspszCmdParam, int nCmdShow)
 *               LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM,
 *                       wParam, LPARAM lParam)
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
 * WinMain creates a windowed application, then dequeues event messages sent to
 *     this application, and forwards them to WndProc, which handles the
 *     messages.
 */
#define STRICT

#include <windows.h>
#include <WindowsX.h>
#include <sstream>
#include <iostream>
#include "main.h"
#include "application.h"




///////////////
// Constants //
///////////////

/** application name */
const char NAME[] = "Comm Shell";

/** default port name */
const char DEFAULT_PORT_NAME[] = "COM3";


/////////////////////////
// Application objects //
/////////////////////////

/** terminal object used to display things */
Terminal* oTerminal;

/** serial port object used to send and receive data */
CommPort* oCommPort;

/** application object that handles many of the windows events */
Application* oApp;

HWND hwnd;
HWND hwndRight;
HWND hwndLeft;
HWND hwndStatus;
HWND hwndEdit;


//////////////////////////
// Forward declarations //
//////////////////////////

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void fnOnReceiveCallback(char c);

//////////////////////////
// Function definitions //
//////////////////////////

/**
 * a windows program that lets one send data out a serial port, and listen to
 *     incoming data from the serial port.
 *
 * @function     WinMain
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
 * A window's program that creates, initializes, and registers a windowed
 *     application with the operating system, and continues on to dequeue
 *     messages from the application's event queue for processing by WndProc.
 *
 * the terminal-like application provides functions accessible in the menu bar
 *     to use with a serial port to send and receive data to and from it
 *
 * @signature    int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 *     LPSTR lspszCmdParam, int nCmdShow)
 *
 * @param        hInst   magical operating system parameter
 * @param        hprevInstance   magical operating system parameter
 * @param        lspszCmdParam   magical operating system parameter
 * @param        nCmdShow   magical operating system parameter
 *
 * @return       exit code
 */
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
      LPSTR lspszCmdParam, int nCmdShow)
{

    // configure & register program
    {
        WNDCLASSEX Wcl;
        Wcl.cbSize = sizeof (WNDCLASSEX);
        Wcl.style = CS_HREDRAW | CS_VREDRAW;
        Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        Wcl.hIconSm = NULL;
        Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
        Wcl.lpfnWndProc = WndProc;
        Wcl.hInstance = hInst;
        Wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
        Wcl.lpszClassName = NAME;
        Wcl.lpszMenuName = "MYMENU";
        Wcl.cbClsExtra = 0;
        Wcl.cbWndExtra = 0;

        if (!RegisterClassEx (&Wcl))
        {
            return 0;
        }
    }

    // create and show the window, and instantiate application objects
    {

        //HFONT hFont =
        //        CreateFont(0, 10, 0, 0, FW_DONTCARE, FALSE, FALSE,  // hFont
        //                FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        //                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
        //                TEXT("Courier New"));

        //FontColors defaultFontColors = {
        //        RGB(255, 255, 0),   // textColor
        //        RGB(0, 0, 0)        // backgroundColor
        //};

        HWND hwnd = CreateWindow(NAME, NAME, WS_OVERLAPPEDWINDOW, 10, 10,
                815, 700, NULL, NULL, hInst, NULL);
        

        // Create panels
        HDC hdcMain = GetDC(hwnd);
        RECT rect;
        GetClientRect(hwnd, &rect);

        int leftWidth = 300;
        int rightWidth = 300;

        HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);

        // Create an Sent box
        HWND hwndLeft = CreateWindowEx (WS_EX_CLIENTEDGE,
            "EDIT",                             // String or Class name from the Register class
            "",                                 // Window name
            WS_CHILD | WS_VSCROLL |
            WS_HSCROLL | WS_VISIBLE |
            ES_MULTILINE | ES_READONLY |
            ES_AUTOHSCROLL | ES_AUTOVSCROLL,    // Style
            rect.left + 20,                     // Initial horizontal position
            rect.top + 20,                      // Initial vertical position
            leftWidth,                          // width
            430,                                // height
            hwnd,                               // parent [optional]
            NULL,                               // menu [optional]
            GetModuleHandle(NULL),              // instance handle
            NULL);                              // CreateStruct [optional]

        // Create an Received box
        HWND hwndRight = CreateWindowEx (WS_EX_CLIENTEDGE,
            "EDIT",                             // String or Class name from the Register class
            "",                                 // Window name
            WS_CHILD | WS_VSCROLL | WS_HSCROLL |WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOHSCROLL | ES_AUTOVSCROLL,   // Style
            rect.left + 20 + 15 + leftWidth,           // Initial horizontal position
            rect.top + 20,                        // Initial vertical position
            rightWidth,                           // width
            430,     // height
            hwnd,                                 // parent [optional]
            NULL,                                 // menu [optional]
            GetModuleHandle(NULL),                            // instance handle
            NULL);                                // CreateStruct [optional]
        
        // Create an Stats box
        HWND hwndStats = CreateWindowEx (WS_EX_CLIENTEDGE,
            "EDIT",                            // String or Class name from the Register class
            "",                              // Window name
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,   // Style
            650,           // Initial horizontal position
            rect.top + 20,                        // Initial vertical position
            130,                           // width
            430,     // height
            hwnd,                                 // parent [optional]
            NULL,                                 // menu [optional]
            GetModuleHandle(NULL),                            // instance handle
            NULL);                                // CreateStruct [optional]

        // Create an input box
        hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
            "EDIT",
            "",
            WS_CHILD|WS_VISIBLE|
            ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
            20,
            rect.bottom - 145,
            615,
            60,
            hwnd,
            (HMENU)IDC_MAIN_EDIT,
            GetModuleHandle(NULL),
            NULL);

        SendMessage(hwndEdit,
            WM_SETFONT,
            (WPARAM)hfDefault,
            MAKELPARAM(FALSE,0));

        // Create a push button
        HWND hWndButton=CreateWindowEx(NULL,
            "BUTTON",
            "Send",
            WS_TABSTOP|WS_VISIBLE|
            WS_CHILD|BS_DEFPUSHBUTTON,
            650,
            rect.bottom - 145,
            130,
            60,
            hwnd,
            (HMENU)IDC_MAIN_BUTTON,
            GetModuleHandle(NULL),
            NULL);
        SendMessage(hWndButton,
            WM_SETFONT,
            (WPARAM)hfDefault,
            MAKELPARAM(FALSE,0));

        // Create an status bar
        HWND hwndStatus = CreateWindowEx(WS_EX_CLIENTEDGE,                       // no extended styles
            "EDIT",         // name of status bar class
            "",           // no text when first created
            WS_CHILD |WS_VSCROLL| WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,   // creates a visible child window
            20,
            rect.bottom - 60,
            leftWidth + rightWidth + 15 + 15 + 130,
            40,              
            hwnd,              // handle to parent window
            NULL,       // child window identifier
            GetModuleHandle(NULL),                   // handle to application instance
            NULL);  
        
        // create objects
        oTerminal = new Terminal(&hwnd, &hwndLeft, &hwndRight, &hwndStatus);
        oCommPort = new CommPort(DEFAULT_PORT_NAME);
        oApp = new Application(hwnd, oCommPort, oTerminal);


        // set the application's mode & comm port
        (*oApp).fnSetMode(ApplicationConsts::Mode::COMMAND);
        (*oApp).fnSelectPort((*oCommPort).fnGetPortName());

        // show the window
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }

    {
        // dequeue messages from the application's event queue & process them
        MSG Msg;
        while (GetMessage (&Msg, NULL, 0, 0))
        {
            TranslateMessage (&Msg);
            DispatchMessage (&Msg);
        }

        // clean up objects
        delete oApp;
        delete oCommPort;
        delete oTerminal;

        // return exit code
        return Msg.wParam;
    }
}

/**
 * event message handler of this application
 *
 * @function     WndProc
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
 * parse and forward messages generated for this application to appropriate
 *     message handling code
 *
 * @signature    LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM
 *     wParam, LPARAM lParam)
 *
 * @param        hwnd   handle to the application's window
 * @param        Message   message generated by the window's OS to be processed
 * @param        wParam   general information associated with Message
 * @param        lParam   additional information associated with Message
 *
 * @return       exit code
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
        WPARAM wParam, LPARAM lParam)
{

    HDC hdc;                // device context handle for painting to the window
    PAINTSTRUCT paintstruct;// used by windows system...
    RECT clientRectangle;   // rectangle containing dimensions of client area

    switch (Message)
    {
        case WM_CREATE:
        {

        }
        break;
        
        ///////////////////////////////
        // Process menu bar messages //
        ///////////////////////////////
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_MAIN_BUTTON:
                {
                    MessageBox(NULL, "HIT ENTER", "INFO", NULL);
                            break;
                }
                ///////////////////
                // set port name //
                ///////////////////
                case IDM_COM1:
                case IDM_COM2:
                case IDM_COM3:
                case IDM_COM4:
                case IDM_COM5:
                case IDM_COM6:
                case IDM_COM7:
                case IDM_COM8:
                case IDM_COM9:
                {
                    // parse and build the port name
                    int commNumber = LOWORD(wParam) - IDM_COM1 + 1;
                    std::stringstream sstm;
                    sstm << "COM" << commNumber;
                    std::string portName = sstm.str();

                    // set the port name
                    (*oApp).fnSelectPort(portName);
                    break;
                }

                ///////////////
                // open port //
                ///////////////
                case IDM_CONNECT:
                    (*oApp).fnSetMode(ApplicationConsts::Mode::CONNECT);
                    break;

                ////////////////
                // close port //
                ////////////////
                case IDM_COMMAND:
                    (*oApp).fnSetMode(ApplicationConsts::Mode::COMMAND);
                    break;

                ////////////////////
                // configure port //
                ////////////////////
                case IDM_CONFIGURE_PORT:
                    (*oApp).fnConfigurePort();
                    break;

                ///////////////
                // open help //
                ///////////////
                case IDM_HELP:
                    (*oApp).fnHelp();
                    break;

                //////////////////
                // clear screen //
                //////////////////
                case IDM_CLEAR_SCREEN:
                    (*oTerminal).fnClearScreen();
                    break;
            }
            break;

        ///////////////////////////////
        // Process keystroke message //
        ///////////////////////////////
        case WM_CHAR:
        {
            char c = (char) wParam;
            /*switch (c)
            {
                case 'a':
                (*oApp).fnStartControlThread();
                break;

                case 's':
                (*oApp).fnStopControlThread();
                break;
            }*/
            /*if ((*oApp).fnGetMode() == ApplicationConsts::Mode::CONNECT)
            {
                if (c != VK_ESCAPE)
                {
                    (*oApp).fnSend((char) wParam);
                }
                else
                {
                    (*oApp).fnSetMode(ApplicationConsts::Mode::COMMAND);
                }
            }*/
            break;
        }

        /////////////////////////////
        // Process repaint message //
        /////////////////////////////
        case WM_PAINT:

            // get device context for painting
            hdc = BeginPaint (hwnd, &paintstruct);

            // Print labels
            TextOut(hdc, 20, 0, "Sent", 4);
            TextOut(hdc, 335, 0, "Received", 8);
            TextOut(hdc, 650, 0, "Stats", 5);
            TextOut(hdc, 20, 475, "Input", 5);
            TextOut(hdc, 20, 560, "Status", 6);

            // release device context...
            EndPaint (hwnd, &paintstruct);
            break;

        /////////////////////////////////
        // Process termination message //
        /////////////////////////////////
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        //////////////////////////////////////////////////////
        // propagate other messages to OS's message handler //
        //////////////////////////////////////////////////////
        default:
            return DefWindowProc (hwnd, Message, wParam, lParam);
    }

    // return...
    return 0;
}

/**
 * forward received callbacks to the application's receive function
 *
 * @function     fnOnReceiveCallback
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
 * @signature    void fnOnReceiveCallback(char c)
 *
 * @param        c   character received through the serial port
 */
void fnOnReceiveCallback(char c)
{
    (*oApp).fnOnReceive(c);
}
