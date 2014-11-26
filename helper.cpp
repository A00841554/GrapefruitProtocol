/*
validatePacket----- NOT DONE YET !! -----> need CRC first
packetizeData------ DONE !! ---> need some final changes (buffer)
checkDuplicate----- DONE !!
processData-------- DONE !!
isEOT-------------- DONE !!
sendData----------- NOT DONE YET !!
CRC---------------- NOT DONE YET !!
*//*
const int headerSize = 2;
const int dataSize = 1018;
const int validationSize = 4; //32 bits
const int packetSize = headerSize + dataSize + validationSize;
*/
#include <fstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "helper.h"

using namespace std;

// Packetizes the data
//char* packetizeData(st_transmit &transmit, bool forceEOT) { //forceEOT = is eot needed to be put here
char* fnPacketizeData() { 
    char* cPacket = new char[iPacketSize];
    char cCurrData[iDataSize] = "";
/*
    currData = cut up to the first 1018 Bytes from data ---->> not possible yet, no buffer
    update value of Pointer to data
*/
    char cHeader[iHeaderSize];
  
    if (true)//(forceEOT)  // OR PACKET IS NOT FULL ----> Implement later
    {
        cHeader[0] = 'F';//testing only   real one-->//char(4);    // EOT
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

    if (true){//transmit.SYN1) {
        cHeader[1] = char(18);
        cHeader[1] = 'U'; // just testing
        //transmit.SYN1 = false;
    } else {
        cHeader[1] = char(19);
        //transmit.SYN1 = true;
    }
/*
    if(transmit.SYN1) {
        Append SYN1 to header
        Set transmit.SYN1 = false
    } else {
        Append SYN2 to header
        Set transmit.SYN1 = true
    }

    */
    for (int i = 0; i < iDataSize; i++) {
        if (cCurrData[i] == char(0)) {
            cCurrData[i] = char(3); //padding with ETX characters
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
    for (int h = 0; h < headerSize;h++)
        cPacket[h] = cHeader[h];
        
    for (int d = 0; d < dataSize;d++)
        cPacket[d + iHeaderSize] = cCurrData[d];

    for (int v = 0; v < iValidationSize; v++)
        cPacket[v + iHeaderSize + iDataSize] = cTheCRC[v];
      
    return cPacket;

} // End of packetizeData

bool checkDuplicate (char cPacket[], st_receive receive) 
{
    //if both are SYN1 (dc2)
    if (cPacket[1] == char(18) && receive.SYN1)
        return true;
    //if both are SYN2 (dc3)
    if ( cPacket[1] == char(19) && !receive.SYN1 )
        return true;

    //if its not a repeated packet
    receive.SYN1 = !receive.SYN1;
    return false;
}

// Check if data is EOT
bool isEOT( char packet[] )
{
    if (packet[0] == char(4))
        return true;
    else 
        return false;
}

// Check if data is ETB
bool fnIsEOT( char cPacket[] )
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

//THIS IS NOT FIXED YET !! 
// -------------------------------------We need to use HANDLE instead of ofstream&
void fnSendData(char cPacket[], std::ofstream& commPort) {
    for (int i = 0; i < iPacketSize; i++)
       commPort << cPacket[i];
}
 
int main () {
    char str1[iPacketSize];
    strcpy(str1, packetizeData());
    
    //isctrl(the character
    cout << "The full packet ---> '"; 
    
    for (int i= 0 ; i < iPacketSize;i++)
        printf("%c",str1[i]);
        
    cout << "'\nThe processed data:  '";
    processData(str1);
    cout << "'";
    
    //testing purposes
    ofstream myfile ("example.txt");
    sendData(str1, myfile);
        
    return 0;
}