/**
 * receivethread is responsible with for acknowledging, receiving,
 * and processing data being transmitted.
 *
 * @sourceFile   receivethread.h
 *
 * @program      Grapefruit.exe
 *
 * @class        n/a
 *
 * @function     DWORD WINAPI fnReceiveThreadIdle(LPVOID lpArg);
 *               DWORD WINAPI fnReceiveThreadActive(LPVOID lpArg);
 *               void fnGoActive(ReceiveArgs* pTransmit);
 *               void fnStop(ReceiveArgs* pTransmit);
 *
 * @date         2014-11-19
 *
 * @revision     0.0.0
 *
 * @designer     Marc Rafanan
 *               Eric Tsang
 *
 * @programmer   Marc Rafanan
 *               Eric Tsang
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
