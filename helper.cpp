/**
 * Helper functions that will be used by the other parts of the program
 *
 * @sourceFile      helper.cpp
 *
 * @program
 *
 * @classes         n/a
 *
 * @functions
 *                  char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
 *                  bool fnValidatePacket(char byPacket[])
 *                  bool checkDuplicate(char byPacket[], ReceiveArgs &receive)
 *                  bool fnIsEOT( char byPacket[] )
 *                  bool fnIsETB( char byPacket[] )
 *                  void fnProcessData(char byPacket[])
 *                  void fnSendData(char byControlChar, HANDLE hCommPort)
 *                  void fnSendData(char byPacket[], HANDLE hCommPort)
 *                  int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout)
 *
 *
 * @date            November 19th, 2014
 *
 * @revisions
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes           none
 */

#include "helper.h"

using namespace std;


/**
 * @function    fnPacketizeData     -> gets a buffer from TransmitArgs and then
 *                                      reads the written file and creates a packet.
 *
 * @date        November 19th, 2014
 *
 * @revision                         * Started with "hardcoded" values just for test *
 *              November 24th, 2014 -> Added the CRC part of the program
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
 *
 * @param       transmit            -> A struct which takes care of the transmit part of our program
 *              packet              -> A PACKET_SIZE sized character array where the packet will be written to
 *              bForceEOT           -> A boolean that checks if the transmit thread wants to end Transmission
 *
 * @return      Char array(the packet)
 *
 * @note
 *
 */
void fnPacketizeData(TransmitArgs &transmit, char* byPacket, bool bForceEOT)
{
    // allocate memory for packet and its parts
    char* byHeader   = byPacket;
    char* byCurrData = byHeader + HEADER_SIZE;
    char* byTheCRC   = byCurrData + DATA_SIZE;
    TransmitBuffer* pTransmitBuffer = transmit.pTransmitBuffer;

    // fill packet with data(and padding)
    for(int i = 0; i < DATA_SIZE; i++)
    {
        if(i < transmit.pTransmitBuffer->size())
        {
            byCurrData[i] = transmit.pTransmitBuffer->at(i);
        }
        else
        {
            byCurrData[i] = ETX;
        }
    }

    // build packet header
    if(bForceEOT || pTransmitBuffer->size() < DATA_SIZE)
    {
        byHeader[0] = EOT;
    }
    else
    {
        byHeader[0] = ETB;
    }

    //check if its equal to previous packet
    if(transmit.bSYN1)
    {
        byHeader[1] = SYN1;
        transmit.bSYN1 = false;
    }
    else
    {
        byHeader[1] = SYN2;
        transmit.bSYN1 = true;
    }

    //get the CRC
    crcInit();
    crc crc = crcFast((unsigned char*) byCurrData, DATA_SIZE);
    memcpy(&byTheCRC[0], &crc, sizeof(crc));

    //group up all the pieces to create a packet
    for(int h = 0; h < HEADER_SIZE;h++)
        byPacket[h] = byHeader[h];

    for(int d = 0; d < DATA_SIZE;d++)
        byPacket[d + HEADER_SIZE] = byCurrData[d];

    for(int v = 0; v < VALIDTION_SIZE; v++)
        byPacket[v + HEADER_SIZE + DATA_SIZE] = byTheCRC[v];
} // End of fnPacketizeData


void fnDropHeadPacketData(TransmitArgs* pTransmit)
{
    auto packetStart = pTransmit->pTransmitBuffer->begin();
    auto packetEnd =(pTransmit->pTransmitBuffer->size() > DATA_SIZE) ?
            pTransmit->pTransmitBuffer->begin() + DATA_SIZE :
            pTransmit->pTransmitBuffer->end();
    pTransmit->pTransmitBuffer->erase(packetStart, packetEnd);
}

void fnAddHeadPacketData(TransmitArgs* pTransmit, char* byPacket)
{
    for(int i = DATA_SIZE + 1; i >= 0; --i)
    {
        pTransmit->pTransmitBuffer->insert(
                pTransmit->pTransmitBuffer->begin(), byPacket[HEADER_SIZE+i]);
    }
}

/**
 * @function    fnValidatePacket    -> gets a packet which is then comparing the CRC in the packet
 *                                      to the calculated CRC and returns a boolean based on how
 *                                      the comparison went.
 *
 * @date        November 21st, 2014
 *
 * @revision
 *              November 24th, 2014 ->  Added the CRC part
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   bool fnValidatePacket(char byPacket[])
 *
 * @param       byPacket[]          -> The packet that will be validated.
 *
 * @return      boolean             -> The result of the comparison as to whether there were errors or not
 *
 * @note
 *
 */
bool fnValidatePacket(char byPacket[]) {

    char* byPacketData = byPacket + HEADER_SIZE;
    char* byPacketCrc = byPacketData + DATA_SIZE;

    // apply the CRC
    crcInit();
    crc syndrome = crcFast((unsigned char*) byPacketData, DATA_SIZE);

    return syndrome == *(crc*) byPacketCrc;
}


/**
 * @function    fnCheckDuplicate    -> checks if the current packet sent is the one intended
 *                                      or a resend of the previous packet.
 *
 * @date        November 24th, 2014
 *
 * @revision
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   bool checkDuplicate(char byPacket[], ReceiveArgs &receive)
 *
 * @param       byPacket[]          -> The packet that will be checked for duplicate.
 *              receive             -> The struct which takes care of the receive part of the program
 *
 * @return      boolean             -> returns the result after knowing if it is meant or not for
 *                                      the current received packet.
 *
 * @note
 *
 */
bool fnCheckDuplicate(char byPacket[], ReceiveArgs &receive)
{
    //if both are bSYN1(dc2)
    if(byPacket[1] == SYN1 && receive.bSYN1)
        return true;
    //if both are bSYN2(dc3)
    if(byPacket[1] == SYN2 && !receive.bSYN1 )
        return true;

    //if its not a repeated packet
    receive.bSYN1 = !receive.bSYN1;
    return false;
}

/**
 * @function    fnIsEOT             -> checks the packet and see if its the last packet or not
 *
 * @date        November 21st, 2014
 *
 * @revision
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   bool fnIsEOT( char byPacket[] )
 *
 * @param       byPacket[]          -> The packet that will be pass on to see if its the last one or not.
 *
 * @return      boolean             -> Checks if the packet has an EOT char, and returns the boolean
 *                                      accordingly.
 *
 * @note
 *
 */
bool fnIsEOT( char byPacket[] )
{
    if(byPacket[0] == EOT)
        return true;
    else
        return false;
}

/**
 * @function    fnIsETB             -> checks the packet and see if its the last packet or not
 *
 * @date        November 24th, 2014
 *
 * @revision
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   bool fnIsETB( char byPacket[] )
 *
 * @param       byPacket[]          -> The packet that will be pass on to see if its the last one or not.
 *
 * @return      boolean             -> Checks if the packet has an ETB char, and returns the boolean
 *                                      accordingly.
 *
 * @note
 *
 */
bool fnIsETB( char byPacket[] )
{
    if(byPacket[0] == ETB)
        return true;
    else
        return false;
}

/**
 * @function    fnProcessData       -> Prints all the characters in the Data part of the
 *                                      packet until an ETX control character is found
 *
 * @date        November 21st, 2014
 *
 * @revision
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *              Marc Rafanan
 *
 * @signature   void fnProcessData(char byPacket[])
 *
 * @param       byPacket[]          -> The packet that is to be printed.
 *
 * @return      void
 *
 * @note
 *
 */
void fnProcessData(char byPacket[])
{
    int iDataend = 0;
    for(int i = HEADER_SIZE; i <(HEADER_SIZE + DATA_SIZE); i++)
    {
        //check if current char being printed is an ETX
        if(byPacket[i] == ETX)
            break;
        iDataend++;
    }

    string str(byPacket);
    string sData = str.substr(HEADER_SIZE, iDataend);
    OutputDebugString(sData.c_str());

    int TextLen = SendMessage(hReceived, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(hReceived, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessage(hReceived, EM_REPLACESEL, FALSE, (LPARAM)sData.c_str());
}


/**
 * @function    fnProcessData       -> Updates the statistics for the session. 
 *                                      
 * @date        November 30th, 2014
 *
 * @revision   
 *
 * @designer    Marc Rafanan
 *
 * @programmer  Marc Rafanan
 *
 * @signature   void fnUpdateStats(const int iStat)
 *
 * @param       iStat           -> Identifier on what to stat to update.
 *
 * @return      void
 *
 * @note        Identifier declarations can be found in protocolparams.h
 *              const int STATS_ACK = 1;
 *              const int STATS_NAK = 2;
 *              const int STATS_INVALID_PCKT = 3;
 *              const int STATS_PCKT_RECEIVED = 4;
 *              const int STATS_PCKT_SENT = 5;
 *
 */
void fnUpdateStats(const int iStat)
{
    switch(iStat)
    {
        case STATS_ACK:
            {
                iAckSent++;
                break;
            }
        case STATS_NAK:
            {
                iNakSent++;
                break;
            }
        case STATS_INVALID_PCKT:
            {
                iInvalidPackets++;
                break;
            }
        case STATS_PCKT_RECEIVED:
            {
                iPacketsReceived++;
                break;
            }
        case STATS_PCKT_SENT:
            {
                iPacketsSent++;
                break;
            }
    }

    // Create output string for the statistic window
    string sStatistics;

    char cNumAck[21];// enough to hold all numbers up to 64-bits
    sprintf(cNumAck, "%d", iAckSent);
    char cNumNak[21];
    sprintf(cNumNak, "%d", iNakSent);
    char cNumInvPckts[21];
    sprintf(cNumInvPckts, "%d", iInvalidPackets);
    char cNumPcktsSent[21];
    sprintf(cNumPcktsSent, "%d", iPacketsSent);
    char cNumPcktsReceived[21];
    sprintf(cNumPcktsReceived, "%d", iPacketsReceived);
    
    int iReceivedErrorRate = (iInvalidPackets == 0 ? 0 : (iPacketsReceived)/iInvalidPackets);
    char cReceivedErrorRate[21];
    sprintf(cReceivedErrorRate, "%d", iReceivedErrorRate);

    sStatistics = "Acks Sent: " + string(cNumAck) +
                  "\r\n\r\nNaks Sent: " + string(cNumNak) +
                  "\r\n\r\nPackets Sent: " + string(cNumPcktsSent) +
                  "\r\n\r\nPackets Received: " + string(cNumPcktsReceived) +
                  "\r\n\r\nInvalid Packets received: " + string(cNumInvPckts) +
                  "\r\n\r\nReceived Packets Error Rate: " + string(cReceivedErrorRate);

    SendMessage(hStats,  WM_SETTEXT, FALSE, (LPARAM)sStatistics.c_str());
}

/**
 * @function    fnSendData          -> Sends a packet to the port
 *
 * @date        November 21st, 2014
 *
 * @revision
 *              November 28th, 2014 -> modified for overlapped
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   void fnSendData(char byPacket[], HANDLE hCommPort)
 *
 * @param       byPacket[]          -> The packet that is to be send.
                hCommPort           -> The handle for the port
 *
 * @return      void
 *
 * @note
 *
 */
void fnSendData(char byPacket[], HANDLE hCommPort)
{
    OutputDebugString("helper: send packet\n");

    OVERLAPPED ov;
    DWORD dwBytesWritten;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    WriteFile(hCommPort, byPacket, PACKET_SIZE, &dwBytesWritten, &ov);

    WaitForSingleObject(ov.hEvent, INFINITE);
    CloseHandle(ov.hEvent);
}

/**
 * @function    fnSendData          -> Sends a control character to the port
 *
 * @date        November 24th, 2014
 *
 * @revision
 *              November 28th, 2014 -> modified for overlapped
 *
 * @designer    Jonathan Chu
 *
 * @programmer  Jonathan Chu
 *
 * @signature   void fnSendData(char byPacket[], HANDLE hCommPort)
 *
 * @param       byControlChar       -> The control character that is to be send.
 *              hCommPort           -> The handle for the port
 *
 * @return      void
 *
 * @note
 *
 */
void fnSendData(char byControlChar, HANDLE hCommPort)
{
    std::stringstream sstm;
    sstm << "helper: send " << int(byControlChar) << endl;
    OutputDebugString(sstm.str().c_str());

    OVERLAPPED ov;
    DWORD dwBytesWritten;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    WriteFile(hCommPort, &byControlChar, 1, &dwBytesWritten, &ov);

    WaitForSingleObject(ov.hEvent, INFINITE);
    CloseHandle(ov.hEvent);
}

/**
 * reads data from the passed serial port handle & returns the value returned by
 *   GetOverlappedResult.
 *
 * @param  hCommPort handle to the serial port
 * @param  pBuffer pointer to the character buffer to have data written into
 * @param  bytesToRead maximum number of characters to write into the address of
 *   the buffer
 * @param  timeout number of milliseconds before the timeout is triggered from
 *   when this function is invoked
 *
 * @return ReadDataResult::TIMEDOUT if the operation timed out,
 *   ReadDataResult::SUCCESS if the read operation succeeded,
 *   ReadDataResult::FAIL if the read operation, ReadDataResult::ERROR if an
 *   error occurred during the reading fails
 */
int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout)
{
    OutputDebugString("Helper: fnReadData\n");
    OVERLAPPED ov;
    DWORD byTransfered;
    int returnCode;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    assert(!ReadFile(hCommPort, pBuffer, bytesToRead, NULL, &ov));

    switch(WaitForSingleObject(ov.hEvent, timeout))
    {

        case WAIT_OBJECT_0:
        if(!GetOverlappedResult(hCommPort, &ov, &byTransfered, TRUE))
        {
            std::stringstream sstm;
            sstm << "fnReadData: Error: " << GetLastError() << endl;
            OutputDebugString(sstm.str().c_str());
            returnCode = ReadDataResult::ERR;
            break;
        }
        else
        {
            returnCode = ReadDataResult::SUCCESS;
            break;
        }

        case WAIT_TIMEOUT:
        CancelIoEx(hCommPort, &ov);
        returnCode = ReadDataResult::TIMEDOUT;
        break;

        default:
        returnCode = ReadDataResult::FAIL;
        break;
    }

    CloseHandle(ov.hEvent);
    return returnCode;
}

int fnWaitForChar(
        HANDLE hCommPort,
        char expectedChar,
        DWORD timeout)
{
    OutputDebugString("fnWaitForChar\n");
    Timer timeoutTimer;
    char readChar;

    timeoutTimer.fnClockStart();
    switch(fnReadData(hCommPort, &readChar, 1, timeout))
    {

        case ReadDataResult::ERR:
        return ReadDataResult::ERR;

        case ReadDataResult::SUCCESS:
        if(readChar == expectedChar)
        {
            return ReadDataResult::SUCCESS;
        }
        else
        {
            DWORD newTimeout = timeout - timeoutTimer.fnTimeElapsed();
            return fnWaitForChar(hCommPort, expectedChar, newTimeout);
        }

        case ReadDataResult::TIMEDOUT:
        return ReadDataResult::TIMEDOUT;

        case ReadDataResult::FAIL:
        return ReadDataResult::FAIL;
    }
    return 0;
}

/**
 * waits for a specified set of characters, and returns when one of them is
 *   received.
 *
 * @param  hCommPort handle to serial port to read from
 * @param  readChar where the received character is written to
 * @param  expectedChars character array of all acceptable characters that we
 *   can receive through the serial port
 * @param  expectedCharsLen length of the passed character array (expectedChars)
 * @param  timeout milliseconds to wait for any of the specified characters
 *
 * @return [file_header] [class_header] [description]
 */
int fnWaitForChars(
        HANDLE hCommPort,
        char* readChar,
        char* expectedChars,
        int expectedCharsLen,
        DWORD timeout)
{
    OutputDebugString("fnWaitForChars\n");
    Timer timeoutTimer;

    timeoutTimer.fnClockStart();
    switch(fnReadData(hCommPort, readChar, 1, timeout))
    {

        case ReadDataResult::ERR:
        return ReadDataResult::ERR;

        case ReadDataResult::SUCCESS:
        {
            for(int i = 0; i < expectedCharsLen; ++i)
            {
                if(*readChar == expectedChars[i])
                {
                    return ReadDataResult::SUCCESS;
                }
            }
            DWORD newTimeout = timeout - timeoutTimer.fnTimeElapsed();
            if(newTimeout > 0)
            {
                return fnWaitForChars(
                        hCommPort,
                        readChar,
                        expectedChars,
                        expectedCharsLen,
                        newTimeout);
            }
            else
            {
                return ReadDataResult::TIMEDOUT;
            }
        }

        case ReadDataResult::TIMEDOUT:
        return ReadDataResult::TIMEDOUT;

        case ReadDataResult::FAIL:
        return ReadDataResult::FAIL;
    }
    return 0;
}

//int main(void)
//{
//    return 0;
//}
