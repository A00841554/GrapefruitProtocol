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
