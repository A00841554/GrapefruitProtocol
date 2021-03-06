/**
 * Helper functions that will be used by the other parts of the program
 *
 * @sourceFile      helper.cpp
 *
 * @program         Grapefruit.exe
 *
 * @classes         n/a
 *
 * @functions
 *                  char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
 *                  bool fnValidatePacket(char byPacket[])
 *                  bool checkDuplicate(char byPacket[], ReceiveArgs &receive)
 *                  void fnDropHeadPacketData(TransmitArgs* pTransmit)
 *                  bool fnIsEOT( char byPacket[] )
 *                  bool fnIsETB( char byPacket[] )
 *                  void fnUpdateStats(const int iStat)
 *                  void fnProcessData(char byPacket[])
 *                  void fnSentData(char byPacket[])
 *                  void fnSendData(char byControlChar, HANDLE hCommPort)
 *                  void fnSendData(char byPacket[], HANDLE hCommPort)
 *                  int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout)
 *                  int fnWaitForChars(HANDLE hCommPort, char* readChar, char* expectedChars, int expectedCharsLen, DWORD timeout)
 *                  int fnWaitForChar(HANDLE hCommPort, char expectedChar, DWORD timeout)
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
 *              byPacket              -> A PACKET_SIZE sized character array where the packet will be written to
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


/**
* @function    fnDropHeadPacketData     -> gets a buffer from TransmitArgs and then
*                                           removes the data that already has been used
*
* @date        December 1st, 2014
*
* @revision    
*
* @designer    Eric Tsang
*
* @programmer  Eric Tsang
*
* @signature   void fnDropHeadPacketData(TransmitArgs* pTransmit)
*
* @param       transmit            -> A struct which takes care of the transmit part of our program
*              
* @return      void
*
* @note
*
*/
void fnDropHeadPacketData(TransmitArgs* pTransmit)
{
    auto packetStart = pTransmit->pTransmitBuffer->begin();
    auto packetEnd =(pTransmit->pTransmitBuffer->size() > DATA_SIZE) ?
            pTransmit->pTransmitBuffer->begin() + DATA_SIZE :
            pTransmit->pTransmitBuffer->end();
    pTransmit->pTransmitBuffer->erase(packetStart, packetEnd);
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

    int TextLen = SendMessage(hReceived, WM_GETTEXTLENGTH, 0, 0);
    int dataLength = sData.length();

    if (TextLen + dataLength > MAX_WIN_CHARS )
    {
        char* sRollOver = new char[TextLen + 1];
        
        GetWindowText(hReceived, sRollOver, TextLen+1);
        SetWindowText(hReceived, NULL);
        sData = sData + sRollOver;
    }
    SendMessage(hReceived, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessage(hReceived, EM_REPLACESEL, FALSE, (LPARAM)sData.c_str());
}


/**
 * @function    fnSentData       -> Prints all the characters in the Data part of the
 *                                  sent packet until an ETX control character is found
 *
 * @date        November 21st, 2014
 *
 * @revision
 *              November 28th, 2014 -> changed char(int) to ETB, EOT, etc..
 *
 * @designer    Marc Rafanan
 *
 * @programmer  Marc Rafanan
 *
 * @signature   void fnSentData(char byPacket[])
 *
 * @param       byPacket[]          -> The packet that is to be printed.
 *
 * @return      void
 *
 * @note
 *
 */
void fnSentData(char byPacket[])
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

    int TextLen = SendMessage(hSent, WM_GETTEXTLENGTH, 0, 0);
    int dataLength = sData.length();

    if (TextLen + dataLength > MAX_WIN_CHARS )
    {
        char* sRollOver = new char[TextLen + 1];
        
        GetWindowText(hSent, sRollOver, TextLen+1);
        SetWindowText(hSent, NULL);
        sData = sData + sRollOver;
    }
    SendMessage(hSent, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessage(hSent, EM_REPLACESEL, FALSE, (LPARAM)sData.c_str());
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
    sprintf_s(cNumAck, "%d", iAckSent);
    char cNumNak[21];
    sprintf_s(cNumNak, "%d", iNakSent);
    char cNumInvPckts[21];
    sprintf_s(cNumInvPckts, "%d", iInvalidPackets);
    char cNumPcktsSent[21];
    sprintf_s(cNumPcktsSent, "%d", iPacketsSent);
    char cNumPcktsReceived[21];
    sprintf_s(cNumPcktsReceived, "%d", iPacketsReceived);
    
    double iReceivedErrorRate = (iInvalidPackets == 0 ? 0 : (iPacketsReceived)/iInvalidPackets);
    char cReceivedErrorRate[21];
    sprintf_s(cReceivedErrorRate, "%d", iReceivedErrorRate);

    sStatistics = "Acks Sent: " + string(cNumAck) +
                  "\r\n\r\nNaks Sent: " + string(cNumNak) +
                  "\r\n\r\nPackets Sent: " + string(cNumPcktsSent) +
                  "\r\n\r\nPackets Received: " + string(cNumPcktsReceived) +
                  "\r\n\r\nInvalid Packets received: " + string(cNumInvPckts) +
                  "\r\n\r\nReceived Packets Error Ratio: " + string(cReceivedErrorRate);

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
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if(!WriteFile(hCommPort, byPacket, PACKET_SIZE, NULL, &ov))
    {
        WaitForSingleObject(ov.hEvent, INFINITE);
    }

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
    //std::stringstream sstm;
    //sstm << "helper: send " << int(byControlChar) << endl;
    //OutputDebugString(sstm.str().c_str());

    OVERLAPPED ov;
    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if(!WriteFile(hCommPort, &byControlChar, 1, NULL, &ov))
    {
        WaitForSingleObject(ov.hEvent, INFINITE);
    }

    CloseHandle(ov.hEvent);
}

/**
 * @function    fnReadData        -> Sends a control character to the port
 *
 * @date        December 2nd, 2014
 *
 * @revision
 *
 * @designer    Eric Tsang
 *
 * @programmer  Eric Tsang
 *
 * @signature   int fnReadData(HANDLE hCommPort, char* pBuffer, DWORD bytesToRead, DWORD timeout)
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
    OVERLAPPED ov;
    DWORD byTransfered;
    int returnCode;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if(ReadFile(hCommPort, pBuffer, bytesToRead, NULL, &ov))
    {
        return ReadDataResult::SUCCESS;
    }

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

/**
* @function    fnWaitForChar       
*
* @date        December 2nd, 2014
*
* @revision
*
* @designer    Eric Tsang
*
* @programmer  Eric Tsang
*
* @signature int fnWaitForChar( HANDLE hCommPort, char expectedChar, DWORD timeout)
* waits for a specified character, and returns when one of them is
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
int fnWaitForChar(
        HANDLE hCommPort,
        char expectedChar,
        DWORD timeout)
{
    Timer timeoutTimer;
    char readChar;

    timeoutTimer.fnClockStart();
    while(timeoutTimer.fnTimeElapsed() < timeout)
    {
        switch(fnReadData(hCommPort, &readChar, 1, timeout - timeoutTimer.fnTimeElapsed()))
        {
            
            case ReadDataResult::SUCCESS:
            if(readChar == expectedChar)
            {
                return ReadDataResult::SUCCESS;
            }
            else
            {
                continue;
            }

            case ReadDataResult::TIMEDOUT:
            return ReadDataResult::TIMEDOUT;

            case ReadDataResult::ERR:
            case ReadDataResult::FAIL:
            assert(false);
            return ReadDataResult::FAIL;
        }   
    }
    return 0;
}

 /**
 * @function    fnWaitForChars         
 *
 * @date        December 2nd, 2014
 *
 * @revision
 *
 * @designer    Eric Tsang
 *
 * @programmer  Eric Tsang
 *
 * @signature int fnWaitForChar( HANDLE hCommPort, char expectedChar, DWORD timeout)
 * waits for a specified character, and returns when one of them is
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