const int headerSize = 2;
const int dataSize = 1018;
const int validationSize = 4; //32 bits
const int packetSize = headerSize + dataSize + validationSize;

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

char* packetizeData();
bool checkDuplicate (char packet[], st_receive receive);
bool fnIsEOT( char packet[] );
void processData(char packet[]);
void sendData(char packet[], std::ofstream& commPort);