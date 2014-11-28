#ifndef _HELPER_H_
#define _HELPER_H_

#include "controlthread.h"

const int iHeaderSize = 2;
const int iDataSize = 1018;
const int iValidationSize = 4; //32 bits
const int iPacketSize = iHeaderSize + iDataSize + iValidationSize;

char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT);
bool fnCheckDuplicate (char cPacket[], ReceiveArgs &receive);
bool fnIsEOT( char cPacket[] );
bool fnIsETB( char cPacket[] );
void fnProcessData(char cPacket[]);
void fnSendData(char cPacket[], std::ofstream& commPort);

#endif