#ifndef _HELPER_H_
#define _HELPER_H_

#include "controlthread.h"

const int HEADER_SIZE = 2;
const int DATA_SIZE = 1018;
const int VALIDTION_SIZE = 4; //32 bits
const int PACKET_SIZE = HEADER_SIZE + DATA_SIZE + VALIDTION_SIZE;

unsigned char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT);
bool fnCheckDuplicate (unsigned char byPacket[], ReceiveArgs &receive);
bool fnIsEOT( unsigned char byPacket[] );
bool fnIsETB( unsigned char byPacket[] );
void fnProcessData(unsigned char byPacket[]);
void fnSendData(unsigned char byPacket[], std::ofstream& commPort);
void fnSendData(unsigned char byControlChar, HANDLE hCommPort);

#endif