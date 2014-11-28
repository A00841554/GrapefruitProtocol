#ifndef _HELPER_H_
#define _HELPER_H_

#include "controlthread.h"

const int HEADER_SIZE = 2;
const int DATA_SIZE = 1018;
const int VALIDTION_SIZE = 4; //32 bits
const int PACKET_SIZE = HEADER_SIZE + DATA_SIZE + VALIDTION_SIZE;

char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT);
bool fnCheckDuplicate (char cPacket[], ReceiveArgs &receive);
bool fnIsEOT( char cPacket[] );
bool fnIsETB( char cPacket[] );
void fnProcessData(char cPacket[]);
void fnSendData(char cPacket[], std::ofstream& commPort);

#endif