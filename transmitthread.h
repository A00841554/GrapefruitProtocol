/**
 * declarations of functions and constants used in functions.cpp
 *
 * @sourceFile transmitthread.h
 *
 * @program    Grapefruit.exe
 *
 * DWORD WINAPI fnTransmitIdle(LPVOID lpArg)
 * DWORD WINAPI fnTransmitActive(LPVOID lpArg)
 * void _TransmitThread_::fnGoActive(TransmitArgs* pTransmit)
 * void _TransmitThread_::fnReset(TransmitArgs* pTransmit)
 * void _TransmitThread_::fnStop(TransmitArgs* pTransmit)
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

#ifndef _TRANSMITTHREAD_H_
#define _TRANSMITTHREAD_H_

#include "controlthread.h"
#include "protocolparams.h"
#include "helper.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

DWORD WINAPI fnTransmitIdle(LPVOID lpArg);
DWORD WINAPI fnTransmitActive(LPVOID lpArg);

namespace _TransmitThread_
{
    void fnGoActive(TransmitArgs* pTransmit);
    void fnReset(TransmitArgs* pTransmit);
    void fnStop(TransmitArgs* pTransmit);
}

#endif
