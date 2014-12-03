/**
 * Terminal class that controls the status and window clearing
 *
 * @sourceFile   terminal.cpp
 *
 * @program      Grapefruit.exe
 *
 * @classes      Terminal
 * 
 * @constructor  Terminal::Terminal(HWND* hwnd, HWND* hwndLeft, 
                                    HWND* hwndRight, HWND* hwndEdit,
                                    HWND* hwndStatus, HWND* hwndStatistics)
 *
 * @function     void Terminal::fnPrint(std::string str)
 * @function     void Terminal::fnClearScreen(void)
 *
 * @date         2014-09-27
 *
 * @revision     none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         none
 */
#include "terminal.h"


/**
 * instantiates a Terminal object.
 *
 * @class        Terminal
 *
 * @method       Terminal
 *
 * @date         2014-09-25
 *
 * @revision     2014-11-19 Modified for Grapefruit Protocol
 *
 * @designer     EricTsang
 *               Marc Rafanan
 *
 * @programmer   EricTsang
 *               Marc Rafanan
 *
 * @note         none
 *
 * @signature    Terminal(HWND* hwnd, HWND* hwndLeft, HWND* hwndRight,
                          HWND* hwndEdit, HWND* hwndStatus, HWND* hwndStatistics)
 *
 * @param        HWND* hwnd         Pointer to the main window handle
 *
 * @param        HWND* hwndLeft     Pointer to the sent messages window
 *
 * @param        HWND* hwndRight    Pointer to the received messages window
 *
 * @param        HWND* hwndEdit     Pointer to the Edit box
 *
 * @param        HWND* hwndStatus   Pointer to the Status bar
 *
 * @param        HWND* hwndStatistics   Pointer to the Stats window
 *
 */
Terminal::Terminal(HWND* hwnd, HWND* hwndLeft, 
                   HWND* hwndRight, HWND* hwndEdit,
                   HWND* hwndStatus, HWND* hwndStatistics)
{
    hwndMain = hwnd;
    hwndSent = hwndLeft;
    hwndReceived = hwndRight;
    hwndEditBox = hwndEdit;
    hwndStsBar = hwndStatus;
    hwndStats = hwndStatistics;
}

/**
 * prints the status to the status window
 *
 * @class        Terminal
 *
 * @method       fnPrint
 *
 * @date         2014-11-19
 *
 * @revision
 *
 * @designer     EricTsang
 *               Marc Rafanan
 *
 * @programmer   EricTsang
 *               Marc Rafanan
 *
 * @note
 *
 * @signature    void Terminal::fnPrint(std::string str)
 *
 * @param        str   string to be printed to the status window
 */
void Terminal::fnPrint(std::string str)
{
    int TextLen = SendMessage(*hwndStsBar, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(*hwndStsBar, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessage(*hwndStsBar, EM_REPLACESEL, FALSE, (LPARAM)str.c_str());
}

/**
 * clears the Sent, Received, and Status window
 *
 * @class        Terminal
 *
 * @method       fnClearScreen
 *
 * @date         2014-11-19
 *
 * @revision
 *
 * @designer     EricTsang
 *               Marc Rafanan
 *
 * @programmer   EricTsang
 *               Marc Rafanan
 *
 * @note         none
 *
 * @signature    void Terminal::fnClearScreen(void)
 */
void Terminal::fnClearScreen(void)
{
    SetWindowText(*hwndStsBar, NULL);   // Clear Status bar
    SetWindowText(*hwndSent, NULL);     // Clear Sent window
    SetWindowText(*hwndReceived, NULL); // Clear Received window
}

