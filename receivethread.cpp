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
    // TO DO readIdle
    // Clear receive buffer

    DWORD dwCommEvent;
    DWORD dwRead;
    char  cRead;
    char strOutputDebugBuffer[MAX_PATH+1] = {0};

    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    OutputDebugString("fnReceiveThreadIdle: Started\n");

    // For OutputDebugString 
    DWORD dwErr = GetLastError();
    char strErrorBuffer[MAX_PATH+1] = {0};

    // for testing purposes, createfile here
    /*stReceive->hCommPort = CreateFile( "COM1",
        GENERIC_READ | GENERIC_WRITE,
        0,    // exclusive access 
        NULL, // default security attributes 
        OPEN_EXISTING,
        NULL,
        NULL 
        );

    if (stReceive->hCommPort == INVALID_HANDLE_VALUE) 
    {
        // Handle the error.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: CreateFile failed with error: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    DCB dcbPortSettings;
    DWORD lastError;
    
    
    // Set default Settings (move to session later)
    if (!GetCommState(stReceive->hCommPort, &dcbPortSettings))
    {
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: Error getting commstate: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    BuildCommDCB("96,N,8,1", &dcbPortSettings);

    if(!SetCommState(stReceive->hCommPort, &dcbPortSettings))
    {
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: Error setting commstate: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }*/
    // end of test


    // SetCommMask to wait for ACK
    if(!SetCommMask(stReceive->hCommPort, EV_RXCHAR))
    {
        // Error setting communications event mask.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: Error SetCommMask: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    while(true)
    {
        if(stReceive->bRequestStop == true)
        {
            stReceive->bStopped = true;
            return 0;
        }

        // Wait for event from the commport
        //if(WaitCommEvent(stReceive->hCommPort, &dwCommEvent, NULL))
        //{
            ReadFile(stReceive->hCommPort, &cRead, 1, &dwRead, NULL);
            if (dwRead > 0)
            {
                
                sprintf_s(strOutputDebugBuffer,  MAX_PATH, "fnReceiveThreadIdle: Detected Char: %c\n", cRead);
                OutputDebugString(strOutputDebugBuffer);
                // Check if chRead is an ENQ
                //if(cRead == 5)
                if(cRead == 48) // press 0 to test
                {
                    if((stReceive->pTransmit)->bActive == true)
                    {
                        stReceive->bStopped = true;
                    }
                    else
                    {
                        stReceive->bActive = true;
                        fnReceiveThreadActive(stReceive);
                    }
                    return 0;
                }
            }
        //}
        //else
        //{
        //    // Error in WaitCommEvent.
        //    break;
        //}
    }

    // for debugging
    // comment out this part
    // CloseHandle(stReceive->hCommPort);

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

    DWORD dwCommEvent;
    DWORD dwRead;
    char  cRead[1024] = {0};
    char  strOutputDebugBuffer[2048] = {0};

    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    // stop the transmit thread
    (stReceive->pTransmit)->bStopped = true;
    // sendData(ACK)
    OutputDebugString("fnReceiveThreadActive: Started\n");

    // for testing purposes only
    DWORD dwErr = GetLastError();
    char strErrorBuffer[MAX_PATH+1] = {0};

        // SetCommMask to wait for ACK
    if(!SetCommMask(stReceive->hCommPort, EV_RXCHAR))
    {
        // Error setting communications event mask.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadActive: Error SetCommMask: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    // Set timeouts 
    // Comments: Constants should be put somewhere
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 3000;
    timeouts.ReadTotalTimeoutMultiplier = 2000;
    timeouts.ReadTotalTimeoutConstant = 1000;
    //timeouts.WriteTotalTimeoutMultiplier = 10;
    //timeouts.WriteTotalTimeoutConstant = 100;

    if (!SetCommTimeouts(stReceive->hCommPort, &timeouts))
    {
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadActive: Error SetCommTimeouts: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    int ctr = 0;
    while(true)
    {
        if(stReceive->bRequestStop == true)
        {
            stReceive->bStopped = true;
            return 0;
        }
        
        sprintf_s(strOutputDebugBuffer,  MAX_PATH, "fnReceiveThreadActive: looping: %d\n", ctr++);
        OutputDebugString(strOutputDebugBuffer);

        if(ReadFile(stReceive->hCommPort, &cRead, 10, &dwRead, NULL)) // size will be 1024
        {
            if(dwRead == 10)
            {
                sprintf_s(strOutputDebugBuffer,  MAX_PATH, "fnReceiveThreadActive: char received: %s %d\n", cRead, dwRead);
                OutputDebugString(strOutputDebugBuffer);

                // Check data if valid; bail if invalid
                // if(!validatePacket(receivedData)) {
                //     sendData(NAK)
                //     continue
                // }

                // ACK packet unless we want to RVI
                //if(stReceive->bRVI == false)
                //{
                    //sendData(ACK)
                //}

                // process the data if it's not a duplicate...
                //if(checkDuplicate(receivedData, recieve) == false) {

                    // receivedData is not duplicate; process data
                //    processData(receivedData)
                //}

                // exit receive thread if EOT or we want to RVI
                //if(checkDuplicate(receivedData, recieve) == true
                //        and receive.RVI == true or isEOT(receiveData)) {
                //    break
                //}
            }
            else if(dwRead < 10)
            {
                OutputDebugString("fnReceiveThreadActive: ReadFile Timed-out\n");
                break;
            }
        }
    }

    stReceive->bStopped = true;
    stReceive->bActive = false;

    return 0;
} // End of fnReceiveThreadActive