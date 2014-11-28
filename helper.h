#ifndef _HELPER_H_
#define _HELPER_H_

#include "controlthread.h"
#include "protocolparams.h"

char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT);
bool fnCheckDuplicate (char byPacket[], ReceiveArgs &receive);
bool fnValidatePacket(char byPacket[]);
bool fnIsEOT( char byPacket[] );
bool fnIsETB( char byPacket[] );
void fnProcessData(char byPacket[]);
void fnSendData(char byPacket[], HANDLE commPort);
void fnSendData(char byControlChar, HANDLE hCommPort);
int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout);

namespace ReadDataResult
{
    static const int TIMEDOUT = 0;
    static const int SUCCESS  = 1;
    static const int FAIL     = 2;
    static const int ERR      = 3;
}

#endif
