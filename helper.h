const int iHeaderSize = 2;
const int iDataSize = 1018;
const int iValidationSize = 4; //32 bits
const int iPacketSize = iHeaderSize + iDataSize + iValidationSize;

struct st_transmit {
    bool request_stop;  // request to stop transmit thread
    bool stopped;       // flag if transmit thread is stopped
    bool active;        // flag if transmit state is active

    bool reset;         // flag if transmit state should be reset
    bool SYN1;          // flag if current data is SYN1
};

struct st_receive {
    bool request_stop;  // request to stop transmit thread
    bool stopped;       // flag if transmit thread is stopped
    bool active;        // flag if transmit state is active

    bool reset;         // flag if transmit state should be reset
    bool SYN1;          // flag if current data is SYN1
};

char* fnPacketizeData();
bool fnCheckDuplicate (char cPacket[], st_receive receive);
bool fnIsEOT( char cPacket[] );
bool fnIsETB( char cPacket[] );
void fnProcessData(char cPacket[]);
void fnSendData(char cPacket[], std::ofstream& commPort);