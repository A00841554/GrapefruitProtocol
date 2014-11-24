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
char* packetizeData() { 
    char* packet = new char[packetSize];
    char currData[dataSize] = "";
/*
    currData = cut up to the first 1018 Bytes from data ---->> not possible yet, no buffer
    update value of Pointer to data
*/
    char header[headerSize];
  
    if (true)//(forceEOT)  // OR PACKET IS NOT FULL ----> Implement later
    {
        header[0] = 'F';//testing only   real one-->//char(4);    // EOT
    }
     else 
    {
        header[0] = char(23);    // ETB
    }

    /*
    initialize header
    if(updated data size == 0 of forceEOT)
        Add EOT to header
    else
        Add ETB to header
    */

    if (true){//transmit.SYN1) {
        header[1] = char(18);
        header[1] = 'U'; // just testing
        //transmit.SYN1 = false;
    } else {
        header[1] = char(19);
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
    for (int i = 0; i < dataSize; i++) {
        if (currData[i] == char(0)) {
            currData[i] = char(3); //padding with ETX characters
        }  
    }
       
    /*
    if(currData < 1018 Bytes)
        Add ETX and padding to the end of currData to complete 1018 Bytes
    */

    char theCRC[validationSize];
    /*
    Get CRC value by calling CRC(currData)
    */
        
    //group up all the pieces to create a packet 
    for (int h = 0; h < headerSize;h++)
        packet[h] = header[h];
        
    for (int d = 0; d < dataSize;d++)
        packet[d + headerSize] = currData[d];

    for (int v = 0; v < validationSize; v++)
        packet[v + headerSize + dataSize] = theCRC[v];
      
    return packet;

} // End of packetizeData

bool checkDuplicate (char packet[], st_receive receive) 
{
    //if both are SYN1 (dc2)
    if (packet[1] == char(18) && receive.SYN1)
        return true;
    //if both are SYN2 (dc3)
    if ( packet[1] == char(19) && !receive.SYN1 )
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


// processData will process the received valid data
void processData(char packet[]) 
{
    for (int i = headerSize; i < (headerSize + dataSize); i++)
    {
        //check if current char being printed is an ETX
        if (packet[i] == char(3))
            return;
        // if not ETX then print
        cout << packet[i];
    }
}

//THIS IS NOT FIXED YET !! 
// -------------------------------------We need to use HANDLE instead of ofstream&
void sendData(char packet[], std::ofstream& commPort) {
    for (int i = 0; i < packetSize; i++)
       commPort << packet[i];
}
 
int main () {
    char str1[packetSize];
    strcpy(str1, packetizeData());
    
    //isctrl(the character
    cout << "The full packet ---> '"; 
    
    for (int i= 0 ; i < packetSize;i++)
        printf("%c",str1[i]);
        
    cout << "'\nThe processed data:  '";
    processData(str1);
    cout << "'";
    
    //testing purposes
    ofstream myfile ("example.txt");
    sendData(str1, myfile);
        
    return 0;
}