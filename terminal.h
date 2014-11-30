
#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * Terminal class function declarations and constant definitions. function
 *                   definitions are in Terminal.cpp
 *
 * @sourceFile   terminal.h
 *
 * @program      RFIDReader.exe
 *
 * @class        Terminal
 *
 * @function     none
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
#include <windows.h>
#include <sstream>
#include <vector>

/** describes the colors used to print a string with */
typedef struct
{
    COLORREF textColor; // color to use for characters
    COLORREF backColor; // color to use for the background of the characters
} FontColors;

/**
 * number of pixes between the the allocated ractangle for this terminal to
 * print in, and the painted border of this terminal
 */
const static int TERMINAL_PADDING = 2;

/**
 * number of pixels between the painted border of the terminal, and the letters
 * within the terminal
 */
const static int TERMINAL_TEXT_PADDING = 3;

/**
 * provides methods to display and manage text on a specified part of the window
 *
 * @class        Terminal
 *
 * @method       Terminal::Terminal(HWND* hwnd, HFONT* prtHFont, FontColors*
 *                   ptrDefaultFontColors)
 * @method       void Terminal::fnAppendString(std::string str, FontColors*
 *                   ptrFontColors)
 * @method       void Terminal::fnPrint(std::string str, FontColors*
 *                   ptrFontColors)
 * @method       void Terminal::fnBackspace(void)
 * @method       void Terminal::fnClearScreen(void)
 * @method       void Terminal::fnRedrawScreen(RECT* rect = NULL, HDC hdc =
 *                   NULL)
 * @method       void Terminal::fnSetClearScreenBeforeNextPrint(void)
 * @method       void Terminal::fnPrintText(HDC hdc, RECT* displayArea, int
 *                   startIndex)
 *
 * @date         2014-09-26
 *
 * @revisions    none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         provides interface methods to display text on a specified
 *                   region of a window's client area. the region of where to
 *                   display information can be specified through the
 *                   fnRedrawScreen function.
 */
class Terminal
{
    public:
        Terminal();
        Terminal(HWND* hwnd, HWND* hwndLeft, HWND* hwndRight, HWND* hwndEdit, HWND* hwndStatus, HWND* hwndStatistics);
        void fnPrint(std::string);
        void fnClearScreen(void);

        /** handle to window that things are displayed on */
        HWND* hwndMain, *hwndSent, *hwndReceived, *hwndEditBox, *hwndStsBar, *hwndStats;
};

extern Terminal mainTerminal;

#endif
