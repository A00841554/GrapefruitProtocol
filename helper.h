/**
 * Definitions for the functions for the helper.cpp are included in here
 *
 * @sourceFile      helper.h
 *
 * @program      
 *
 * @classes         n/a
 *
 * @functions       none
 *
 * @date            November 21st, 2014
 *
 * @revisions
 *                  November 21st, 2014 ->  added fnIsETB( char byPacket[] )
 *                                          added fnCheckDuplicate (char byPacket[], ReceiveArgs &receive)
 *                                          added void fnSendData(char byControlChar, HANDLE hCommPort)
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 * 
 */
#ifndef _HELPER_H_
#define _HELPER_H_

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include "crc.h"
#include "terminal.h"
#include "controlthread.h"
#include "protocolparams.h"

void fnPacketizeData(TransmitArgs &transmit, char* packet, bool bForceEOT);
bool fnCheckDuplicate (char byPacket[], ReceiveArgs &receive);
bool fnValidatePacket(char byPacket[]);
bool fnIsEOT( char byPacket[] );
bool fnIsETB( char byPacket[] );
void fnProcessData(char byPacket[]);
void fnSendData(char byPacket[], HANDLE commPort);
void fnSendData(char byControlChar, HANDLE hCommPort);
int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout);
void fnDropHeadPacketData(TransmitArgs&);
void fnAddHeadPacketData(TransmitArgs&, char*);

namespace ReadDataResult
{
    static const int TIMEDOUT = 0;
    static const int SUCCESS  = 1;
    static const int FAIL     = 2;
    static const int ERR      = 3;
}

#endif
