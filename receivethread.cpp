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

    // for testing purposes only
    DWORD dwErr = GetLastError();
    char strErrorBuffer[MAX_PATH+1] = {0};

    stReceive->hCommPort = CreateFile( "COM1",
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
        sprintf_s(strErrorBuffer,  MAX_PATH, "CreateFile failed with error: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    DCB dcbPortSettings;
    DWORD lastError;
    
    
    // Set default Settings (move to session later)
    if (!GetCommState(stReceive->hCommPort, &dcbPortSettings))
    {
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "Error getting commstate: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    BuildCommDCB("96,N,8,1", &dcbPortSettings);

    if(!SetCommState(stReceive->hCommPort, &dcbPortSettings))
    {
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "Error setting commstate: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }
    // end of test


    // SetCommMask to wait for ACK
    if(!SetCommMask(stReceive->hCommPort, EV_RXCHAR))
    {
        // Error setting communications event mask.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "Error SetCommMask: 0x%x\n", dwErr);
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
        if(WaitCommEvent(stReceive->hCommPort, &dwCommEvent, NULL))
        {
            if(ReadFile(stReceive->hCommPort, &cRead, 1, &dwRead, NULL))
            {
                
                sprintf_s(strOutputDebugBuffer,  MAX_PATH, "Detected Char: %c\n", cRead);
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
        }
        else
        {
            // Error in WaitCommEvent.
            break;
        }
    }

    // for debugging
    // comment out this part
    CloseHandle(stReceive->hCommPort);

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
    char  cRead;
    char strOutputDebugBuffer[MAX_PATH+1] = {0};

    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    // stop the transmit thread
    (stReceive->pTransmit)->bStopped = true;
    // sendData(ACK)
    OutputDebugString("fnReceiveThreadActive\n");

    // for testing purposes only
    DWORD dwErr = GetLastError();
    char strErrorBuffer[MAX_PATH+1] = {0};

        // SetCommMask to wait for ACK
    if(!SetCommMask(stReceive->hCommPort, EV_RXCHAR))
    {
        // Error setting communications event mask.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "Error SetCommMask: 0x%x\n", dwErr);
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
        if(WaitCommEvent(stReceive->hCommPort, &dwCommEvent, NULL))
        {
            if(ReadFile(stReceive->hCommPort, &cRead, 1, &dwRead, NULL))
            {
                
                sprintf_s(strOutputDebugBuffer,  MAX_PATH, "Detected Char: %c\n", cRead);
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
                        //fnReceiveThreadActive(stReceive);
                    }
                    return 0;
                }
            }
        }
        else
        {
            // Error in WaitCommEvent.
            break;
        }
    }
    // TO DO readactive


    //while(true)
    //{
        //Wait for event from receive buffer
        //if(waiting times-out) {
        //    break
        //}

        //if data received {
        //    Set receivedData = (data received)

            // Check data if valid; bail if invalid
        //    if(!validatePacket(receivedData)) {
        //        sendData(NAK)
        //        continue
        //    }

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

        //} // End of data is received
    //}// End of while loop

    stReceive->bStopped = true;
    stReceive->bActive = false;

    return 0;
} // End of fnReceiveThreadActive