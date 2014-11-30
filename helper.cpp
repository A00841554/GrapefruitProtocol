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
 *                  bool checkDuplicate (char byPacket[], ReceiveArgs &receive)
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
 *              bForceEOT           -> A boolean that checks if the transmit thread wants to end Transmission
 *
 * @return      Char array (the packet)
 *
 * @note        
 *
 */
char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
{ 
    // allocate memory for packet and its parts
    char* byPacket   = new char[PACKET_SIZE];
    char* byHeader   = byPacket;
    char* byCurrData = byHeader + HEADER_SIZE;
    char* byTheCRC   = byCurrData + DATA_SIZE;
    TransmitBuffer* pTransmitBuffer = transmit.pTransmitBuffer;

    // fill packet with data (and padding)
    for (int i = 0; i < DATA_SIZE; i++)
    {
        if (i < transmit.pTransmitBuffer->size())
        {
            byCurrData[i] = transmit.pTransmitBuffer->at(i);
        }
        else
        {
            byCurrData[i] = ETX;
        }
    }

    // build packet header
    if (bForceEOT || pTransmitBuffer->empty())
    {
        byHeader[0] = EOT;
    }
    else
    {
        byHeader[0] = ETB;
    }

    //check if its equal to previous packet
    if (transmit.bSYN1)
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
    for (int h = 0; h < HEADER_SIZE;h++)
        byPacket[h] = byHeader[h];

    for (int d = 0; d < DATA_SIZE;d++)
        byPacket[d + HEADER_SIZE] = byCurrData[d];

    for (int v = 0; v < VALIDTION_SIZE; v++)
        byPacket[v + HEADER_SIZE + DATA_SIZE] = byTheCRC[v];

    return byPacket;
} // End of fnPacketizeData


void fnDropHeadPacketData(TransmitArgs& transmit)
{
    auto packetStart = transmit.pTransmitBuffer->begin();
    auto packetEnd = transmit.pTransmitBuffer->begin() + DATA_SIZE;
    transmit.pTransmitBuffer->erase(packetStart, packetEnd);
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
 * @signature   bool checkDuplicate (char byPacket[], ReceiveArgs &receive)
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
bool fnCheckDuplicate (char byPacket[], ReceiveArgs &receive)
{
    //if both are bSYN1 (dc2)
    if (byPacket[1] == SYN1 && receive.bSYN1)
        return true;
    //if both are bSYN2 (dc3)
    if (byPacket[1] == SYN2 && !receive.bSYN1 )
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
    if (byPacket[0] == EOT)
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
    if (byPacket[0] == ETB)
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
    OutputDebugString("fnProcessData\n");
    for (int i = HEADER_SIZE; i < (HEADER_SIZE + DATA_SIZE); i++)
    {
        //check if current char being printed is an ETX
        if (byPacket[i] == ETX)
            return;
        // if not ETX then print
        OutputDebugString("fnProcessData0\n");
        int TextLen = SendMessage(*(mainTerminal.hwndReceived), WM_GETTEXTLENGTH, 0, 0);
        OutputDebugString("fnProcessData1\n");
        SendMessage(*(mainTerminal.hwndReceived), EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
        OutputDebugString("fnProcessData2\n");
        SendMessage(*(mainTerminal.hwndReceived), EM_REPLACESEL, FALSE, (LPARAM)byPacket[i]);
        OutputDebugString("fnProcessData3\n");
    }
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
    OutputDebugString("helper: send packet");

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
    std::stringstream sstm;
    sstm << "helper: reading\n";
    OutputDebugString(sstm.str().c_str());

    OVERLAPPED ov;
    DWORD byTransfered;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    assert(!ReadFile(hCommPort, pBuffer, bytesToRead, NULL, &ov));

    switch (WaitForSingleObject(ov.hEvent, timeout))
    {

        case WAIT_OBJECT_0:
        if (!GetOverlappedResult(hCommPort, &ov, &byTransfered, TRUE))
		{
		    DWORD dwErr = GetLastError();
            char strErrorBuffer[MAX_PATH+1] = {0};
            sprintf_s(strErrorBuffer,MAX_PATH,"fnReadData: Error: 0x%x\n",dwErr);
            OutputDebugString(strErrorBuffer);
            return ReadDataResult::ERR;
		}
        else
		{
		    CancelIoEx(hCommPort, &ov);
            CloseHandle(ov.hEvent);
            return ReadDataResult::SUCCESS;
		}

        case WAIT_TIMEOUT:
		CancelIoEx(hCommPort, &ov);
		CloseHandle(ov.hEvent);
        return ReadDataResult::TIMEDOUT;

        default:
		CancelIoEx(hCommPort, &ov);
		CloseHandle(ov.hEvent);
        return ReadDataResult::FAIL;
    }
}

//int main(void)
//{
//    return 0;
//}
