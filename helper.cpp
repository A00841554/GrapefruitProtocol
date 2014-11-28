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

using namespace std;

// Packetizes the data
char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT)
{ //forceEOT = is eot needed to be put here
    char* byPacket = new char[PACKET_SIZE];
    char byCurrData[DATA_SIZE] = "";
/*
    currData = cut up to the first 1018 Bytes from data ---->> not possible yet, no buffer
    update value of Pointer to data
*/
    char byHeader[HEADER_SIZE];

    if (bForceEOT)  // OR PACKET IS NOT FULL ----> Implement later
    {
        byHeader[0] = char(4);    // EOT
    }
     else
    {
        byHeader[0] = char(23);    // ETB
    }

    /*
    initialize header
    if(updated data size == 0 of forceEOT)
        Add EOT to header
    else
        Add ETB to header
    */

    if (transmit.bSYN1)
    {
        byHeader[1] = char(18);
        transmit.bSYN1 = false;
    }
    else
    {
        byHeader[1] = char(19);
        transmit.bSYN1 = true;
    }
/*
    if(transmit.bSYN1) {
        Append bSYN1 to header
        Set transmit.bSYN1 = false
    } else {
        Append bSYN2 to header
        Set transmit.bSYN1 = true
    }

    */
    for (int i = 0; i < DATA_SIZE; i++)
    {
        if (byCurrData[i] == char(0))
        {  // if null
            byCurrData[i] = char(3);     //padding with ETX characters
        }
    }

    /*
    if(currData < 1018 Bytes)
        Add ETX and padding to the end of currData to complete 1018 Bytes
    */

    char byTheCRC[VALIDTION_SIZE];
    /*
    Get CRC value by calling CRC(currData)
    */

    //group up all the pieces to create a packet
    for (int h = 0; h < HEADER_SIZE;h++)
        byPacket[h] = byHeader[h];

    for (int d = 0; d < DATA_SIZE;d++)
        byPacket[d + HEADER_SIZE] = byCurrData[d];

    for (int v = 0; v < VALIDTION_SIZE; v++)
        byPacket[v + HEADER_SIZE + DATA_SIZE] = byTheCRC[v];

    return byPacket;

} // End of packetizeData

bool checkDuplicate (char byPacket[], ReceiveArgs &receive)
{
    //if both are bSYN1 (dc2)
    if (byPacket[1] == char(18) && receive.bSYN1)
        return true;
    //if both are bSYN2 (dc3)
    if (byPacket[1] == char(19) && !receive.bSYN1 )
        return true;

    //if its not a repeated packet
    receive.bSYN1 = !receive.bSYN1;
    return false;
}

// Check if data is EOT
bool fnIsEOT( char byPacket[] )
{
    if (byPacket[0] == char(4))
        return true;
    else
        return false;
}

// Check if data is ETB
bool fnIsETB( char byPacket[] )
{
    if (byPacket[0] == char(23))
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
        if (byPacket[i] == char(3))
            return;
        // if not ETX then print
        cout << byPacket[i];
    }
}

void fnSendData(char byPacket[], HANDLE hCommPort)
{
    OutputDebugString("TransmitThread: send packet");

    OVERLAPPED ov;
    DWORD dwBytesWritten;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    WriteFile(hCommPort, byPacket, PACKET_SIZE, &dwBytesWritten, NULL);

    WaitForSingleObject(ov.hEvent, INFINITE);
}

void fnSendData(char byControlChar, HANDLE hCommPort)
{
    std::stringstream sstm;
    sstm << "TransmitThread: send " << int(byControlChar) << endl;
    OutputDebugString(sstm.str().c_str());

    OVERLAPPED ov;
    DWORD dwBytesWritten;

    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    WriteFile(hCommPort, &byControlChar, 1, &dwBytesWritten, &ov);

    WaitForSingleObject(ov.hEvent, INFINITE);
}

//int main(void)
//{
//    return 0;
//}
