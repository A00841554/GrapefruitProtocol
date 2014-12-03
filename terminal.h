
#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * Terminal class function declarations and constant definitions. function
 *                   definitions are in Terminal.cpp
 *
 * @sourceFile   terminal.h
 *
 * @program      Grapefruit.exe
 *
 * @class        Terminal
 *
 * @function     void Terminal::fnPrint(std::string str)
 * @function     void Terminal::fnClearScreen(void)
 *
 * @date         2014-09-27
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
 */
#include <windows.h>
#include <sstream>
#include <vector>

class Terminal
{
    public:
        Terminal(HWND* hwnd, HWND* hwndLeft, HWND* hwndRight, HWND* hwndEdit, HWND* hwndStatus, HWND* hwndStatistics);
        void fnPrint(std::string);
        void fnClearScreen(void);

        /** handle to window that things are displayed on */
        HWND* hwndMain, *hwndSent, *hwndReceived, *hwndEditBox, *hwndStsBar, *hwndStats;
};

/* Handles for the Main window and panels */
extern HWND hMain;
extern HWND hSent;
extern HWND hReceived;
extern HWND hEdit;
extern HWND hStats;
extern HWND hStatusBar;

/* Statistic values */
extern int iAckSent;
extern int iNakSent;
extern int iInvalidPackets;
extern int iPacketsSent;
extern int iPacketsReceived;

#endif
