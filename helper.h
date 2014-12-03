/**
 * Definitions for the functions for the helper.cpp are included in here
 *
 * @sourceFile      helper.h
 *
 * @program         Grapefruit.exe
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
 * @designer        Jonathan Chu, Marc Rafanan
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
#include "timer.h"
#include "terminal.h"
#include "controlthread.h"
#include "protocolparams.h"

// This is the max number of characters in the send/receive window
const int MAX_WIN_CHARS     = 25000;

// the number of characters to roll over when window is full
const int ROLL_OVER_CHARS   = 5000;

void fnPacketizeData(TransmitArgs &transmit, char* packet, bool bForceEOT);
bool fnCheckDuplicate (char byPacket[], ReceiveArgs &receive);
bool fnValidatePacket(char byPacket[]);
bool fnIsEOT( char byPacket[] );
bool fnIsETB( char byPacket[] );

void fnProcessData(char byPacket[]);
void fnSentData(char byPacket[]);
void fnUpdateStats(const int iStat);
void fnDropHeadPacketData(TransmitArgs*);

void fnSendData(char byPacket[], HANDLE commPort);
void fnSendData(char byControlChar, HANDLE hCommPort);
int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout);
int fnWaitForChar(HANDLE hCommPort, char expectedChar, DWORD timeout);
int fnWaitForChars(HANDLE hCommPort, char* readChar, char* expectedChars, int expectedCharsLen, DWORD timeout);

namespace ReadDataResult
{
    static const int TIMEDOUT = 0;
    static const int SUCCESS  = 1;
    static const int FAIL     = 2;
    static const int ERR      = 3;
}

#endif
