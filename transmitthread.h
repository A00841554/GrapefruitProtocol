#ifndef _TRANSMITTHREAD_H_
#define _TRANSMITTHREAD_H_

#include "controlthread.h"
#define SHORT_SLEEP 100
#define MAX_MISS 3
#define MAX_SENT 10
#define ACK 6
#define NAK 21
#define RVI 17
#include <cstdlib>
#include <ctime>
#include <algorithm>

DWORD WINAPI fnTransmitIdle(LPVOID lpArg);
DWORD WINAPI fnTransmitActive(LPVOID lpArg);

#endif