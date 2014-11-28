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
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "helper.h"

using namespace std;

// Packetizes the data
char* fnPacketizeData(TransmitArgs &transmit, bool bForceEOT) 
{ //forceEOT = is eot needed to be put here
    char* cPacket = new char[PACKET_SIZE];
    char cCurrData[DATA_SIZE] = "";
/*
    currData = cut up to the first 1018 Bytes from data ---->> not possible yet, no buffer
    update value of Pointer to data
*/
    char cHeader[HEADER_SIZE];
  
    if (bForceEOT)  // OR PACKET IS NOT FULL ----> Implement later
    {
        cHeader[0] = char(4);    // EOT
    }
     else 
    {
        cHeader[0] = char(23);    // ETB
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
        cHeader[1] = char(18);
        transmit.bSYN1 = false;
    } 
    else 
    {
        cHeader[1] = char(19);
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
    for (int i = 0; i < iDataSize; i++) 
    {
        if (cCurrData[i] == char(0)) 
        {  // if null
            cCurrData[i] = char(3);     //padding with ETX characters
        }  
    }
       
    /*
    if(currData < 1018 Bytes)
        Add ETX and padding to the end of currData to complete 1018 Bytes
    */

    char cTheCRC[iValidationSize];
    /*
    Get CRC value by calling CRC(currData)
    */
        
    //group up all the pieces to create a packet 
    for (int h = 0; h < iHeaderSize;h++)
        cPacket[h] = cHeader[h];
        
    for (int d = 0; d < iDataSize;d++)
        cPacket[d + iHeaderSize] = cCurrData[d];

    for (int v = 0; v < iValidationSize; v++)
        cPacket[v + iHeaderSize + iDataSize] = cTheCRC[v];
      
    return cPacket;

} // End of packetizeData

bool checkDuplicate (char cPacket[], ReceiveArgs &receive) 
{
    //if both are bSYN1 (dc2)
    if (cPacket[1] == char(18) && receive.bSYN1)
        return true;
    //if both are bSYN2 (dc3)
    if ( cPacket[1] == char(19) && !receive.bSYN1 )
        return true;

    //if its not a repeated packet
    receive.bSYN1 = !receive.bSYN1;
    return false;
}

// Check if data is EOT
bool fbIsEOT( char cPacket[] )
{
    if (cPacket[0] == char(4))
        return true;
    else 
        return false;
}

// Check if data is ETB
bool fnIsETB( char cPacket[] )
{
    if (cPacket[0] == char(23))
        return true;
    else 
        return false;
}

// processData will process the received valid data
void fnProcessData(char cPacket[]) 
{
    for (int i = iHeaderSize; i < (iHeaderSize + iDataSize); i++)
    {
        //check if current char being printed is an ETX
        if (cPacket[i] == char(3))
            return;
        // if not ETX then print
        cout << cPacket[i];
    }
}

void fnSendData(char cPacket[], HANDLE hCommPort) 
{
    DWORD dwBytesWritten;
    WriteFile(hCommPort, cPacket, iPacketSize, &dwBytesWritten, NULL);
}

int main(void)
{
//    return 0;
//}