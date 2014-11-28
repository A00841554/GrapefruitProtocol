/*
validatePacket----- NOT DONE YET !! -----> need CRC first
packetizeData------ DONE !! ---> need some final changes (buffer)
checkDuplicate----- DONE !!
processData-------- DONE !!
isEOT-------------- DONE !!
sendData----------- DONE !!
CRC---------------- NOT DONE YET !!
*/
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "helper.h"
//#include "crc.h"

using namespace std;

// Packetizes the data
char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
{ //forceEOT = is eot needed to be put here
    // allocate memory for packet
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
    /*
    Get CRC value by calling CRC(currData)
    */
    // eric commented out crc code, because he couldnt find it on his machine
    //crcInit();
    //char byTheCRC[VALIDTION_SIZE] = crcFast(byCurrData,strlen(byCurrData));

    //group up all the pieces to create a packet
    for (int h = 0; h < HEADER_SIZE;h++)
        byPacket[h] = byHeader[h];

    for (int d = 0; d < DATA_SIZE;d++)
        byPacket[d + HEADER_SIZE] = byCurrData[d];

    for (int v = 0; v < VALIDTION_SIZE; v++)
        byPacket[v + HEADER_SIZE + DATA_SIZE] = byTheCRC[v];

    return byPacket;

} // End of packetizeData

void fnDropHeadPacketData(TransmitArgs& transmit)
{
    auto packetStart = transmit.pTransmitBuffer->begin();
    auto packetEnd = transmit.pTransmitBuffer->begin() + DATA_SIZE;
    transmit.pTransmitBuffer->erase(packetStart, packetEnd);
}

bool fnValidatePacket(char byPacket[]) {

    char byCurrData[DATA_SIZE] = "";
    
    for (int d = 0; d < DATA_SIZE;d++)
        byCurrData[d] = byPacket[d + HEADER_SIZE];
    
    // eric commented out crc code, because he couldnt find it on his machine
    //crcInit();
    //char byTheCRC[VALIDTION_SIZE] = crcFast(byCurrData,strlen(byCurrData));
    
    for (int v = 0; v < VALIDTION_SIZE; v++)
        if ( byPacket[v + HEADER_SIZE + DATA_SIZE] != byTheCRC[v])
            return false;
            
    return true;
}


bool checkDuplicate (char byPacket[], ReceiveArgs &receive)
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

// Check if data is EOT
bool fnIsEOT( char byPacket[] )
{
    if (byPacket[0] == EOT)
        return true;
    else
        return false;
}

// Check if data is ETB
bool fnIsETB( char byPacket[] )
{
    if (byPacket[0] == ETB)
        return true;
    else
        return false;
}

// processData will process the received valid data
void fnProcessData(char byPacket[])
{
    for (int i = HEADER_SIZE; i < (HEADER_SIZE + DATA_SIZE); i++)
    {
        //check if current char being printed is an ETX
        if (byPacket[i] == ETX)
            return;
        // if not ETX then print
        cout << byPacket[i];
    }
}

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
    DWORD dwRead;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    ReadFile(hCommPort, pBuffer, bytesToRead, &dwRead, &ov);

    switch (WaitForSingleObject(ov.hEvent, timeout))
    {

        case WAIT_OBJECT_0:
        if (!GetOverlappedResult(hCommPort, &ov, &dwRead, true))
		{
		    DWORD dwErr = GetLastError();
            char strErrorBuffer[MAX_PATH+1] = {0};
            sprintf_s(strErrorBuffer,MAX_PATH,"fnReadData: Error: 0x%x\n",dwErr);
            OutputDebugString(strErrorBuffer);
            return ReadDataResult::ERR;
		}
        else
		{
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
