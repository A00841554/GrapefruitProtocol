
/**
 * definitions of functions declared in terminal.h
 *
 * @sourceFile   terminal.cpp
 *
 * @program      DumbTerminal2.exe
 *
 * @classes      Terminal
 *
 * @function     Terminal::Terminal(HWND* hwnd, FontSettings* fontSettings)
 * @function     void Terminal::fnPrint(std::string str)
 * @function     void Terminal::fnBackspace(void)
 * @function     void Terminal::fnClearScreen(void)
 * @function     void Terminal::fnRedrawScreen(RECT* rect = NULL, HDC hdc =
 *                   NULL)
 * @function     void Terminal::fnSetClearScreenBeforeNextPrint(void)
 * @function     void Terminal::fnPrintText(HDC hdc, RECT* displayArea,
 *                   std::string text)
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



/////////////////
// Constructor //
/////////////////
Terminal::Terminal()
{

}
/**
 * instantiates a Terminal object.
 *
 * @class        Terminal
 *
 * @method       Terminal
 *
 * @date         2014-09-25
 *
 * @revision     none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         none
 *
 * @signature    Terminal::Terminal(HWND* hwnd, HFONT* prtHFont, FontColors*
 *                   ptrDefaultFontColors)
 *
 * @param        hwnd   handle to the window that this Terminal instance wil
 *                   print things on
 * @param        fontSettings   settings for the font used to display
 *                   information on this terminal
 * @param        ptrDefaultFontColors   pointer to a FontColors structure that
 *                   describes the default colors used to display some text if
 *                   no other colors are specified
 */
Terminal::Terminal(HWND* hwnd, HWND* hwndLeft, HWND* hwndRight, HWND* hwndEdit, HWND* hwndStatus, HWND* hwndStatistics)
{
    hwndMain = hwnd;
    hwndSent = hwndLeft;
    hwndReceived = hwndRight;
    hwndEditBox = hwndEdit;
    hwndStsBar = hwndStatus;
    hwndStats = hwndStatistics;
}

/**
 * prints the passed string (str) to the Terminal.
 *
 * @class        Terminal
 *
 * @method       fnPrint
 *
 * @date         2014-09-25
 *
 * @revision     2014-10-13 - Eric Tsang - extracted the appending of the string
 *                   (str) to a new function so client objects may append
 *                   multiple strings with different colors using the other
 *                   function, then invoke fnRedrawScreen to update the screen.
 *                   this is faster than calling fnPrint multiple times, because
 *                   fnPrint always invokes fnRedrawScreen
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         prints the passed string (str) to the Terminal. if
 *                   fnSetClearScreenBeforeNextPrint was called before this
 *                   print call, then clear the terminal immediately before
 *                   printing the passed string (str) to the Terminal.
 *
 * @signature    void Terminal::fnPrint(std::string str, FontColors*
 *                   ptrFontColors)
 *
 * @param        str   string to be printed to the Terminal
 * @param        ptrFontColors   pointer to a FontColors struct that describes
 *                   the colors used to display the passed string (str) with
 */
void Terminal::fnPrint(std::string str)
{
    int TextLen = SendMessage(*hwndStsBar, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(*hwndStsBar, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessage(*hwndStsBar, EM_REPLACESEL, FALSE, (LPARAM)str.c_str());
}

/**
 * clears the Terminal screen
 *
 * @class        Terminal
 *
 * @method       fnClearScreen
 *
 * @date         2014-09-25
 *
 * @revision     2014-10-13 - Eric Tsang - clears the saved font colors now too,
 *                   because text colors is a new addition to the terminal
 *                   object
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         none
 *
 * @signature    void Terminal::fnClearScreen(void)
 */
void Terminal::fnClearScreen(void)
{
    SetWindowText(*hwndStsBar, NULL);
}

