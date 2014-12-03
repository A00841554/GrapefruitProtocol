/**
 * receivethread source file
 *
 * @sourceFile   receivethread.cpp
 *
 * @program      Grapefruit.exe
 *
 * @class        n/a
 *
 * @function     n/a
 *
 * @date         2014-11-19
 *
 * @revision     0.0.0
 *
 * @designer     Marc Rafanan
 *
 * @programmer   Marc Rafanan
 *
 * @note         none
 */


#include "receivethread.h"
#include <assert.h>


/**
 * @function   fnReceiveThreadIdle - Idle state for readthread
 *
 * @date       2014-11-19
 *
 * @revision   0.0.0 - Initial draft
 *
 * @designer   Marc Rafanan
 *
 * @programmer Marc Rafanan
 *
 * @signature  DWORD WINAPI fnReceiveThreadIdle(LPVOID lpArg)
 *
 * @param      stReceive - Structure that holds the flags for receivethread
 *
 * @return     DWORD
 *
 * @note       note1
 *
 */
DWORD WINAPI fnReceiveThreadIdle(LPVOID lpArg)
{

    DWORD dwRead;
    DWORD dwErr;
    DWORD dwWait;
    BOOL bWaitReturn = false;
    BOOL bWaitRead = false;

    char  readChar;
    char strOutputDebugBuffer[2048+1] = {0};
    char strErrorBuffer[2048+1] = {0};

    // Set receive structure
    ReceiveArgs* pReceive = (ReceiveArgs*) lpArg;

    // clear receive buffer
    ClearCommError(pReceive->hCommPort, NULL, NULL);
    PurgeComm(pReceive->hCommPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    // set up overlapped structure
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // check for data from serial port, or a request to stop
    OutputDebugString("ReceiveThread: Idle\n");
    HANDLE handles[] = {pReceive->hRequestStop, ov.hEvent};
    while(true)
    {
        ReadFile(pReceive->hCommPort, &readChar, 1, NULL, &ov);
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        switch(result)
        {
            case WAIT_OBJECT_0+0:
            {
                _ReceiveThread_::fnStop(pReceive);
                break;
            }
            case WAIT_OBJECT_0+1:
            {
                if(readChar == ENQ)
                {
                    if(!pReceive->pTransmit->bActive)
                    {
                        _ReceiveThread_::fnGoActive(pReceive);
                    }
                    else
                    {
                        _ReceiveThread_::fnStop(pReceive);
                    }
                }
                else
                {
                    continue;
                }
                break;
            }
            default:
            {
                DWORD err = GetLastError();
                OutputDebugString("Something went wrong...\n");
                _ReceiveThread_::fnStop(pReceive);
                break;
            }
        }
        break;
    }

    CloseHandle(ov.hEvent);

    OutputDebugString("ReceiveThread: Stopped\n");
    return 0;
} // End of fnReceiveIdle

/**
 * @function   fnReceiveThreadActive - Active state for readthread
 *
 * @date       2014-11-19
 *
 * @revision   0.0.0 - Initial draft
 *
 * @designer   Marc Rafanan
 *
 * @programmer Marc Rafanan
 *
 * @signature  DWORD WINAPI fnReceiveThreadActive(LPVOID lpArg)
 *
 * @param      stReceive - Structure that holds the flags for receivethread
 *
 * @return     DWORD
 *
 * @note       note1
 *
 */
DWORD WINAPI fnReceiveThreadActive(LPVOID lpArg)
{

    OutputDebugString("ReceiveThread: Active\n");

    ReceiveArgs* pReceive = (ReceiveArgs*) lpArg;
    char  receivedPacket[PACKET_SIZE+1] = {0};

    // stop the transmit thread
    SetEvent(pReceive->pTransmit->hRequestStop);

    // wait for transmit thread to stop before going full active
    while(!pReceive->pTransmit->bStopped)
    {
        Sleep(SHORT_SLEEP);
    }
    OutputDebugString("ReceiveThread: Full Active\n");

    // ACK the ENQ
    fnSendData(ACK, pReceive->hCommPort);
    fnUpdateStats(STATS_ACK);

    // wait for the packet
    while(true)
    {
        int result = fnReadData(pReceive->hCommPort, receivedPacket, PACKET_SIZE, TIMEOUT_AFTER_R_ENQ);
        switch(result)
        {
            case ReadDataResult::SUCCESS:
            {
                OutputDebugString("ReceiveThread: Received Packet");

                // update UI
                fnUpdateStats(STATS_PCKT_RECEIVED);

                // Check data if valid; bail if invalid
                if(!fnValidatePacket(receivedPacket))
                {
                    OutputDebugString("fnReceiveThreadActive: Invalid data\n");
                    fnSendData(NAK, pReceive->hCommPort);
                    fnUpdateStats(STATS_NAK);
                    fnUpdateStats(STATS_INVALID_PCKT);
                    continue;
                }

                // ACK packet unless we want to RVI
                if(pReceive->bRVI == false)
                {
                    fnSendData(ACK, pReceive->hCommPort);
                }

                // process the data if it's not a duplicate...
                if(!fnCheckDuplicate(receivedPacket, *pReceive))
                {
                    fnProcessData(receivedPacket);
                }

                // exit receive thread if EOT or we want to RVI; continue to receive packets otherwise
                if(/*bDuplicate || */pReceive->bRVI || fnIsEOT(receivedPacket))
                {
                    fnProcessData("HH\r\n----------------------\r\n");
                    _ReceiveThread_::fnStop(pReceive);
                }
                else
                {
                    continue;
                }
                break;
            }
            case ReadDataResult::TIMEDOUT:
            {
                _ReceiveThread_::fnStop(pReceive);
                break;
            }
            default:
            {
                _ReceiveThread_::fnStop(pReceive);
                break;
            }
        }
        break;
    }
    
    return 0;
} // End of fnReceiveThreadActive

void _ReceiveThread_::fnGoActive(ReceiveArgs* pReceive)
{
    SetEvent(pReceive->pTransmit->hRequestStop);
    pReceive->bActive = true;
    fnReceiveThreadActive(pReceive);
}

void _ReceiveThread_::fnStop(ReceiveArgs* pReceive)
{
    pReceive->bActive = false;
    pReceive->bStopped = true;
}
