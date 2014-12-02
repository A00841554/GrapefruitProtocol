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

#ifndef _RECEIVETHREAD_H_
#define _RECEIVETHREAD_H_

#include <assert.h>
#include <stdio.h>
#include "controlthread.h"
#include "commport.h"
#include "helper.h"

DWORD WINAPI fnReceiveThreadIdle(LPVOID lpArg);
DWORD WINAPI fnReceiveThreadActive(LPVOID lpArg);

namespace _ReceiveThread_
{
    void fnGoActive(ReceiveArgs* pTransmit);
    void fnStop(ReceiveArgs* pTransmit);
}

#endif
