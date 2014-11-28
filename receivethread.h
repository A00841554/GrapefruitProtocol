/**
 * receivethread header file
 *
 * @sourceFile   receivethread.h
 *
 * @program      Grapefruit.exe
 *
 * @class        n/a
 *
 * @function     n/a
 *
 * @date         2014-11-19
 *
 * @revision     0.0.0
 *
 * @designer     Marc Rafanan
 *
 * @programmer   Marc Rafanan
 *
 * @note         none
 */

#include<stdio.h>
#include "controlthread.h"
#include "commport.h"

DWORD WINAPI fnReceiveThreadIdle(LPVOID lpArg);
DWORD WINAPI fnReceiveThreadActive(LPVOID lpArg);
