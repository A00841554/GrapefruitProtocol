
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
        Terminal(HWND*, HFONT*, FontColors*);
        void fnAppendString(std::string, FontColors* = NULL);
        void fnPrint(std::string, FontColors* = NULL);
        void fnBackspace(void);
        void fnClearScreen(void);
        void fnRedrawScreen(RECT* = NULL, HDC = NULL);
        void fnSetClearScreenBeforeNextPrint(void);

    private:

        /**
         * text displayed on the Terminal. text is saved so it can be repainted
         *   to the screen as necessary
         */
        std::string mDisplayedText;

        /**
         * array of FontColors that define the background and foreground color
         *   of the character that's printed in the corresponding index
         */
        std::vector<FontColors*> mFontColors;

        /** handle to window that things are displayed on */
        HWND mHwnd;

        /** information about font used on the Terminal */
        HFONT* mPtrHFont;

        /** font colors used if font color is not specified in the print function */
        FontColors* mPtrDefaultFontColors;

        /** client area of the window where the terminal's text is printed */
        RECT mDisplayArea;

        /**
         * when set to true, the next time fnPrint is called, the screen will be
         *   cleared immediately before the new text is printed. once this
         *   happens, the flag is set back to false
         */
        bool mClearScreenNextPrint;
        void fnPrintText(HDC, RECT*, int);
};

#endif
