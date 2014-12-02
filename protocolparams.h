#ifndef _PROTOCOLPARAMS_H_
#define _PROTOCOLPARAMS_H_

// control characters
const char EOT  = char('N');//4
const char ETB  = char('C');//23
const char ETX  = char('X');//3
const char SYN1 = char('S');//18
const char SYN2 = char('Z');//19
const char ACK  = char('A');//6
const char NAK  = char('N');//21
const char RVI  = char('R');//17
const char ENQ  = char('E');//5

// timeouts & retry counts
const int SHORT_SLEEP                   = 100;
const int MAX_MISS                      = 3;
const int MAX_SENT                      = 10;
const int MAX_RESET_TIMEOUT             = 1000;
const int CONTROL_THREAD_SLEEP_INTERVAL = 250;
const int TIMEOUT_AFTER_T_ENQ           = 2000;
const int TIMEOUT_AFTER_T_PACKET        = 2000;

// packets
const int HEADER_SIZE    = 2;
const int DATA_SIZE      = 10;//1018;
const int VALIDTION_SIZE = 4; //32 bits
const int PACKET_SIZE    = HEADER_SIZE + DATA_SIZE + VALIDTION_SIZE;

// statistics
const int STATS_ACK = 1;
const int STATS_NAK = 2;
const int STATS_INVALID_PCKT = 3;
const int STATS_PCKT_RECEIVED = 4;
const int STATS_PCKT_SENT = 5;

#endif
