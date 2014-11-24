/*
validatePacket
packetizeData
checkDuplicate
processData
isEOT
sendData
CRC
*/
const int headerSize = 2;
const int dataSize = 1018;
const int validationSize = 4;
const int packetSize = headerSize + dataSize + validationSize;

#include <fstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>

using namespace std;


struct st_transmit {
    bool request_stop;  // request to stop transmit thread
    bool stopped;       // flag if transmit thread is stopped
    bool active;        // flag if transmit state is active

    bool reset;         // flag if transmit state should be reset
    bool SYN1;          // flag if current data is SYN1
};


void CRC(char yes[]) {
    yes[0] = 'y';
}


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
    // return packet
    Return (header + data + CRC)
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




int main () {

    char str1[packetSize];
    strcpy(str1, packetizeData());
    
    //isctrl(the character
    cout << "--->"; 
    
    
    for (int i= 0 ; i < packetSize;i++)
        printf("%c",str1[i]);
    return 0;
}