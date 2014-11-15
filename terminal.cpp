
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
Terminal::Terminal(HWND* hwnd, HFONT* prtHFont,
        FontColors* ptrDefaultFontColors) {

    // initialize variables
    mDisplayedText = std::string();
    memset(&mDisplayArea, 0, sizeof(RECT));
    mClearScreenNextPrint = false;

    // initialize variables from function parameters
    mHwnd = *hwnd;
    mPtrHFont = prtHFont;
    mPtrDefaultFontColors = ptrDefaultFontColors;
}




/////////////////////////
// Interface functions //
/////////////////////////

/**
 * appends the passed string (str) for printing, but doesn't display it. invoke
 *                   fnRedrawScreen() to display it after.
 *
 * @class        Terminal
 *
 * @method       fnAppendString
 *
 * @date         2014-10-13
 *
 * @revision     none
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         this function was created so one can print multiple strings
 *                   with different colors, and not have to invoke fnPrint
 *                   instead because invoking fnPrint multiple times is very
 *                   expensive since fnPrint updates the UI every time it is
 *                   called
 *
 * @signature    void Terminal::fnAppendString(std::string str, FontColors*
 *                   ptrFontColors)
 *
 * @param        str   std::string to print
 * @param        ptrFontColors   colors used to display the font with
 */
void Terminal::fnAppendString(std::string str, FontColors* ptrFontColors) {

    // resolve arguments
    if (!ptrFontColors) {
        ptrFontColors = mPtrDefaultFontColors;
    }

    // print stuff
    mDisplayedText.append(str);
    for (int i = 0; i < str.length(); i++) {
        mFontColors.push_back(ptrFontColors);
    }
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
void Terminal::fnPrint(std::string str, FontColors* ptrFontColors) {

    // clear the screen if flag mClearScreenNextPrint
    if (mClearScreenNextPrint) {
        mClearScreenNextPrint = false;
        fnClearScreen();
    }

    // print stuff
    fnAppendString(str, ptrFontColors);
    fnRedrawScreen();
}

/**
 * removes a character from the Terminal.
 *
 * @class        Terminal
 *
 * @method       fnBackspace
 *
 * @date         2014-09-25
 *
 * @revision     2014-10-13 - Eric Tsang - removes a character from the
 *                   terminal, as well as a font color
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         removes the character from the Terminal. if this function is
 *                   called when there are no characters on the Terminal to
 *                   remove, then nothing happens.
 *
 * @signature    void Terminal::fnBackspace(void)
 */
void Terminal::fnBackspace(void) {
    int newLength = mDisplayedText.length() - 1;
    if (newLength >= 0) {
        mDisplayedText.resize(newLength);
        mFontColors.resize(newLength);
        fnRedrawScreen();
    }
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
void Terminal::fnClearScreen(void) {
    mDisplayedText.clear();
    mFontColors.clear();
    fnRedrawScreen(&mDisplayArea);
}

/**
 * redraws the screen given the new client area (rect) which defines where txt
 *                   can be printed to.
 *
 * @class        Terminal
 *
 * @method       fnRedrawScreen
 *
 * @date         2014-09-25
 *
 * @revision     2014-10-13 - Eric Tsang - revised code to draw a white
 *                   rectangle around the terminal's drawing area, and use text
 *                   and border padding
 * @revision     2014-10-13 - Eric Tsang - added a try catch block around using
 *                   the hdc for painting because sometimes it throws some kind
 *                   of exception, but this is a rare occurrence.
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         none
 *
 * @signature    void Terminal::fnRedrawScreen(RECT* rect = NULL, HDC hdc =
 *                   NULL)
 *
 * @param        rect   area defining where text for this Terminal can be
 *                   printed
 * @param        hdc   handle to the device context, needed for painting on the
 *                   window
 */
void Terminal::fnRedrawScreen(RECT* rect, HDC hdc) {

    bool releaseHDC = false;

    // resolve arguments
    if (rect == NULL) {
        rect = &mDisplayArea;
    } else {
        mDisplayArea = *rect;
    }
    if (hdc == NULL) {
        hdc = GetDC(mHwnd);
        releaseHDC = true;
    }

    // calculate terminal, and text rectangles
    RECT terminalRect;
    memcpy(&terminalRect, rect, sizeof(RECT));
    terminalRect.top += TERMINAL_PADDING;
    terminalRect.left += TERMINAL_PADDING;
    terminalRect.bottom -= TERMINAL_PADDING;
    terminalRect.right -= TERMINAL_PADDING;

    RECT textRect;
    memcpy(&textRect, &terminalRect, sizeof(RECT));
    textRect.top += TERMINAL_TEXT_PADDING;
    textRect.left += TERMINAL_TEXT_PADDING;
    textRect.bottom -= TERMINAL_TEXT_PADDING;
    textRect.right -= TERMINAL_TEXT_PADDING;

    // draw a black rectangle in the client area to cover everything
    // behind & print our received characters in the client area
    try {
        SelectObject(hdc, GetStockObject(WHITE_PEN));
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        Rectangle(hdc, terminalRect.left, terminalRect.top,
                terminalRect.right, terminalRect.bottom);
        fnPrintText(hdc, &textRect, 0);
    } catch( std::out_of_range ) {}

    // release device context...
    if (releaseHDC) {
        ReleaseDC (mHwnd, hdc);
    }
}

/**
 * sets a flag to clear the screen immediately before the printing the text when
 *                   fnPrint is called
 *
 * @class        Terminal
 *
 * @method       fnSetClearScreenBeforeNextPrint
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
 * @signature    void Terminal::fnSetClearScreenBeforeNextPrint(void)
 */
void Terminal::fnSetClearScreenBeforeNextPrint(void) {
    mClearScreenNextPrint = true;
}




//////////////////////
// Helper functions //
//////////////////////

/**
 * recursive function...prints text to the Terminal screen, in a way that does
 *                   word wrapping if necessary
 *
 * @class        Terminal
 *
 * @method       fnPrintText
 *
 * @date         2014-09-25
 *
 * @revision     2014-10-12 - Eric Tsang - Revised code to now print text with
 *                   more than just one color
 *
 * @designer     EricTsang
 *
 * @programmer   EricTsang
 *
 * @note         none
 *
 * @signature    void Terminal::fnPrintText(HDC hdc, RECT* displayArea, int
 *                   startIndex)
 *
 * @param        hdc   handle to the device context used for painting on the
 *                   window
 * @param        displayArea   area on the client area to print things in
 * @param        startIndex   position in the string mDisplayedText to start
 *                   printing from
 */
void Terminal::fnPrintText(HDC hdc, RECT* displayArea, int startIndex) {

    TEXTMETRIC textMetric;  // printing font's dimensional information
    std::string currRowText = "";   // text to print in current row
    std::string extraText = "";     // text to print in subsequent rows
    size_t divisionIndex;

    // applying fontSettings
    SelectObject(hdc, *mPtrHFont);

    // calculating display area, and font size
    GetTextMetrics(hdc, &textMetric);
    unsigned long displayWidth = (*displayArea).right - (*displayArea).left;
    unsigned long displayHeight = (*displayArea).bottom - (*displayArea).top;
    unsigned long fontWidth = textMetric.tmAveCharWidth;
    unsigned long fontHeight = textMetric.tmHeight;
    unsigned long textWidth = displayWidth / fontWidth - 1;

    // reassign currRowText and extraText
    {
        // determine if we need print text on the next line
        if (mDisplayedText.substr(startIndex).length() > textWidth
                || mDisplayedText.find("\n", startIndex) != std::string::npos) {

            // text is truncated at the first newline character if it exists;
            // truncated at the last space otherwise.
            std::string truncatedText = mDisplayedText.substr(startIndex);
            divisionIndex = truncatedText.find("\n");
            if (divisionIndex == std::string::npos
                    || divisionIndex > textWidth) {
                divisionIndex = truncatedText.find_last_of(" ", textWidth);
            }

            // skip the newline, or space character
            if (divisionIndex != std::string::npos) {
                divisionIndex++;
            }

            // if text was not truncated yet, truncate at
            // maximum text width.
            if (divisionIndex == std::string::npos
                    || divisionIndex > textWidth) {
                divisionIndex = textWidth;
            }

            // assign currRowText & extraText
            currRowText = truncatedText.substr(0, divisionIndex);
            extraText = truncatedText.substr(divisionIndex);
        } else {
            currRowText = mDisplayedText.substr(startIndex);
        }
    }

    // print current row
    for (int i = 0; i < currRowText.length(); i++) {

        // set font color
        FontColors* ptrFontColors = mFontColors.at(i+startIndex);
        SetTextColor(hdc, ptrFontColors->textColor);
        SetBkColor(hdc, ptrFontColors->backColor);

        // do the printing
        TextOut(hdc, (*displayArea).left+(i*fontWidth),
            (*displayArea).top, currRowText.c_str()+i, 1);
    }

    // print the rest of the text in the next rows
    if (extraText != "") {
        RECT tempDisplayArea = *displayArea;
        tempDisplayArea.top = tempDisplayArea.top + fontHeight;
        if (tempDisplayArea.bottom > tempDisplayArea.top + fontHeight) {
            fnPrintText(hdc, &tempDisplayArea, startIndex + divisionIndex);
        }
    }
}
