#ifndef _HELPER_H_
#define _HELPER_H_

#include "controlthread.h"

const int HEADER_SIZE = 2;
const int DATA_SIZE = 1018;
const int VALIDTION_SIZE = 4; //32 bits
const int PACKET_SIZE = HEADER_SIZE + DATA_SIZE + VALIDTION_SIZE;

char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT);
bool fnCheckDuplicate (char byPacket[], ReceiveArgs &receive);
bool fnValidatePacket(char byPacket[]);
bool fnIsEOT( char byPacket[] );
bool fnIsETB( char byPacket[] );
void fnProcessData(char byPacket[]);
void fnSendData(char byPacket[], HANDLE commPort);
void fnSendData(char byControlChar, HANDLE hCommPort);

#endif