#ifndef _TRANSMITTHREAD_H_
#define _TRANSMITTHREAD_H_

#include "controlthread.h"
#define SHORT_SLEEP 100
#include <cstdlib>
#include <ctime>
#include <algorithm>

DWORD WINAPI fnTransmitIdle(LPVOID lpArg);
DWORD WINAPI fnTransmitActive(LPVOID lpArg);

#endif