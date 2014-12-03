/**
 * implementation of functions declared in main.h
 *
 * @sourceFile main.cpp
 *
 * @program    Grapefruit.exe
 *
 * @function   int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int)
 * @function   BOOL CALLBACK EnumChildProc(HWND , LPARAM)
 * @function   LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
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

#define STRICT

#include <windows.h>
#include <WindowsX.h>
#include <sstream>
#include <iostream>
#include "main.h"
#include "application.h"
#include "protocolparams.h"
#include "timer.h"




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

/** Declare global variables */
HWND hMain, hSent, hReceived, hEdit, hStatusBar, hStats;

/** Initialize Statistic values */
int iAckSent = 0;
int iNakSent = 0;
int iInvalidPackets = 0;
int iPacketsSent = 0;
int iPacketsReceived = 0;

//////////////////////////
// Forward declarations //
//////////////////////////

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

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
 * @designer     EricTsang & Marc Rafanan & Jonathan Chu
 *
 * @programmer   EricTsang & Marc Rafanan & Jonathan Chu
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
            WS_VISIBLE |
            ES_MULTILINE | ES_READONLY |
            ES_AUTOVSCROLL,    // Style
            rect.left + 20,                     // Initial horizontal position
            rect.top + 20,                      // Initial vertical position
            leftWidth,                          // width
            430,                                // height
            hwnd,                               // parent [optional]
            (HMENU)SENT_BOX,                               // menu [optional]
            GetModuleHandle(NULL),              // instance handle
            NULL);

        /*
         * If you're just using the Win32 API, you normally handle this by
         *   handling the WM_SIZE message, and respond by calling GetClientRect
         *   on the parent window and MoveWindow on the children to move/resize
         *   the children to fill the parent appropriately
         */

        // Create an Received box
        HWND hwndRight = CreateWindowEx (WS_EX_CLIENTEDGE,
            "EDIT",                 // String or Class name from the Register class
            "",                     // Window name
            WS_CHILD | WS_VSCROLL | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,   // Style
            rect.left + 20 + 15 + leftWidth,    // Initial horizontal position
            rect.top + 20,          // Initial vertical position
            rightWidth,             // width
            430,                    // height
            hwnd,                   // parent [optional]
            (HMENU)RECEIVED_BOX,    // menu [optional]
            GetModuleHandle(NULL),  // instance handle
            NULL);                  // CreateStruct [optional]

        // Create an Stats box
        HWND hwndStats = CreateWindowEx (WS_EX_CLIENTEDGE,
            "EDIT",                 // String or Class name from the Register class
            "",                     // Window name
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY, // Style
            650,                    // Initial horizontal position
            rect.top + 20,          // Initial vertical position
            130,                    // width
            430,                    // height
            hwnd,                   // parent [optional]
            (HMENU)STATS_BOX,       // menu [optional]
            GetModuleHandle(NULL),  // instance handle
            NULL);                  // CreateStruct [optional]

        SendMessage(hwndStats,
            WM_SETFONT,
            (WPARAM)hfDefault,
            MAKELPARAM(FALSE,0));

        // Create an input box
        HWND hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
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
        HWND hwndStatus = CreateWindowEx(WS_EX_CLIENTEDGE,
            "EDIT",                 // name of status bar class
            "",                     // no text when first created
            WS_CHILD |WS_VSCROLL| WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,   // creates a visible child window
            20,                     // position X
            rect.bottom - 60,       // position y
            leftWidth + rightWidth + 15 + 15 + 130,     // X status BAR length
            40,                     // Y status bar length
            hwnd,                   // handle to parent window
            (HMENU)STATUS_BAR,      // child window identifier
            GetModuleHandle(NULL),  // handle to application instance
            NULL);

        // create objects
        oTerminal = new Terminal(&hwnd, &hwndLeft, &hwndRight, &hwndEdit, &hwndStatus, &hwndStats);

        hMain = hwnd;
        hSent = hwndLeft;
        hReceived = hwndRight;
        hEdit = hwndEdit;
        hStatusBar = hwndStatus;
        hStats = hwndStats;

        oCommPort = new CommPort(DEFAULT_PORT_NAME);
        oApp = new Application(hwnd, oCommPort, oTerminal);

        //set the random seed
        srand(time(NULL));

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
 * Handles the updating for the size of our sub windows
 *
 * @function     EnumChildProc
 *
 * @date         November 29th, 2014
 *
 * @revisions    none
 *
 * @designer     Jonathan Chu
 *
 * @programmer   Jonathan Chu
 *
 * @param        lParam        additional information associated with Message
 *                hwndChild    A handler for the childs
 *
 * @return       returns true once its done
 *
 * @notes
 */
BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
    LPRECT theMainWindow;
    int whichPanel;

    // Retrieve the child-window identifier. Use it to set the
    // position of the child window.
    whichPanel = GetWindowLong(hwndChild, GWL_ID);

    // looks for the panel
    theMainWindow = (LPRECT)lParam;

    switch (whichPanel)
    {
        case RECEIVED_BOX:
        {
            MoveWindow(hwndChild,
            ((theMainWindow->right / 5) * 2) + 12,
            20,
            ((theMainWindow->right / 5) * 2) - 10,
            theMainWindow->bottom - 250,
            TRUE);
        }
              break;

        case SENT_BOX:
        {
            MoveWindow(hwndChild,
            10,
            20,
            ((theMainWindow->right / 5) * 2) - 10,
            theMainWindow->bottom - 250,
            TRUE);
        }
             break;

        case STATS_BOX:
        {
            MoveWindow(hwndChild,
                ((theMainWindow->right / 5) * 4) + 15,
            20,
            (theMainWindow->right / 5) - 15,
            theMainWindow->bottom - 250,
            TRUE);
        }
              break;

        case IDC_MAIN_EDIT:
        {
            MoveWindow(hwndChild,
                10,
                theMainWindow->bottom - 210,
                (theMainWindow->right / 5) * 4 - 10,
                80,
                TRUE);
        }
            break;

        case IDC_MAIN_BUTTON:
        {
            MoveWindow(hwndChild,
                ((theMainWindow->right / 5) * 4) + 10,
                theMainWindow->bottom - 220,
                (theMainWindow->right / 5) - 20,
                100,
                TRUE);
        }
            break;
        case STATUS_BAR:
        {
            MoveWindow(hwndChild,
                10,
                theMainWindow->bottom - 110,
                theMainWindow->right - 20,
                100,
                TRUE);
        }
            break;
    }

    // Make sure the child window is visible.
    ShowWindow(hwndChild, SW_SHOW);

    return TRUE;
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
 * @programmer   EricTsang & Marc Rafanan & Jonathan Chu
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


    RECT rcClient;          // CHECK

    switch (Message)
    {
        case WM_CREATE:
        {
            if (RegisterHotKey(
                hwnd,                           //win handler
                321,                            //the ID
                MOD_CONTROL | MOD_NOREPEAT,     // ctrl+ and keeping it pressed won't do anything else
                0x53))                          //0x42 is 's'
            {
              OutputDebugString("Hotkey 'ctr + s' registered, using MOD_NOREPEAT flag\n");
            }
        }
            break;
        //////////////////////
        // Resizable Window //
        //////////////////////
        case WM_SIZE:
            GetClientRect(hwnd, &rcClient);
            EnumChildWindows(hwnd, EnumChildProc, (LPARAM)&rcClient);

            break;


        case WM_HOTKEY:
            if ((*oApp).fnGetMode() == ApplicationConsts::Mode::CONNECT)
            {
                int textLength = GetWindowTextLength(*oTerminal->hwndEditBox);
                char* string = new char[textLength + 1];
                GetWindowText(*oTerminal->hwndEditBox, string, textLength + 1);
                SetWindowText(*oTerminal->hwndEditBox, NULL);
                (*oApp).fnSend(string, textLength);
                delete string;
            }
            OutputDebugString("Hotkey 'ctr + s' pressed\n");
            break;

        ///////////////////////////////
        // Process menu bar messages //
        ///////////////////////////////
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /////////////////////
                // hit send button //
                /////////////////////
                case IDC_MAIN_BUTTON:
                {
                    if((*oApp).fnGetMode() == ApplicationConsts::Mode::CONNECT)
                    {
                        int textLength = GetWindowTextLength(*oTerminal->hwndEditBox);
                        char* string = new char[textLength+1];

                        GetWindowText(*oTerminal->hwndEditBox, string, textLength+1);
                        SetWindowText(*oTerminal->hwndEditBox, NULL);
                        (*oApp).fnSend(string, textLength);

                        delete string;
                    }
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

                ////////////
                // do RVI //
                ////////////
                case IDM_RVI:
                    (*oApp).fnSetRVI();
                    break;
            }
            break;

        /////////////////////////////
        // Process repaint message //
        /////////////////////////////
        case WM_PAINT:

            // get device context for painting
            hdc = BeginPaint (hwnd, &paintstruct);
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);

            // Print labels
            TextOut(hdc, 10, 0, "Sent", 4);
            TextOut(hdc, ((rcClient.right / 5) * 2) + 12, 0, "Received", 8);
            TextOut(hdc, ((rcClient.right / 5) * 4) + 15, 0, "Stats", 5);
            TextOut(hdc, 10, rcClient.bottom - 230, "Input", 5);
            TextOut(hdc, 10, rcClient.bottom - 130, "Status", 6);

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
