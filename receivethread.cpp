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
    stReceive->hCommPort = CreateFile( "COM1",
        GENERIC_READ | GENERIC_WRITE,
        0,    // exclusive access 
        NULL, // default security attributes 
        OPEN_EXISTING,
        //NULL, // non-overlapped
        FILE_FLAG_OVERLAPPED, // Testing for overlapped structure.
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
    }

    // return value for wait
    BOOL bWaitReturn = false;
    BOOL bWaitRead = false;
    DWORD dwWait;

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

    // Create overlapped structure
    OVERLAPPED ov;
    ov.hEvent = CreateEvent(NULL,   // default security attributes
                            TRUE,   // manual-reset event
                            FALSE,  // not signaled
                            NULL);   // no name

    // Initialize the rest of the OVERLAPPED structure to zero.
    ov.Internal = 0;
    ov.InternalHigh = 0;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    BOOL bWaitCommEvent = false;

    while(true)
    {
        if(stReceive->bRequestStop == true)
        {
            stReceive->bStopped = true;
            CloseHandle(stReceive->hCommPort); // Delete this line
            CloseHandle(ov.hEvent); // Close handle for event
            return 0;
        }

        // Wait for event from the commport
        if (!bWaitRead)
        {
            // Issue read operation.
            if (!ReadFile(stReceive->hCommPort, &cRead, 1, &dwRead, &ov)) // Wait for 10 Bytes for now
            {
                dwErr = GetLastError();
                if (dwErr != ERROR_IO_PENDING)     // read not delayed?
                {
                    sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: Error ReadFile: 0x%x\n", dwErr);
                    OutputDebugString(strErrorBuffer);
                    CloseHandle(stReceive->hCommPort); // Delete this line
                    CloseHandle(ov.hEvent); // Close handle for event
                    return dwErr;
                }
                else
                {
                    bWaitRead = TRUE;
                }
            }
            else 
            {
                // Read char on port
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
                    CloseHandle(stReceive->hCommPort); // Delete this line
                    CloseHandle(ov.hEvent); // Close handle for event
                    return 0;
                }
            }
        } // if (!bWaitRead)

        if (bWaitRead) 
        {
            dwWait = WaitForSingleObject(ov.hEvent, 5000); // Change timeout and put it somewhere
            switch(dwWait)
            {
                // Read completed.
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(stReceive->hCommPort, &ov, &dwRead, FALSE))
                    {
                        dwErr = GetLastError();
                        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadIdle: Error GetOverlappedResult: 0x%x\n", dwErr);
                        OutputDebugString(strErrorBuffer);
                        CloseHandle(stReceive->hCommPort); // Delete this line
                        CloseHandle(ov.hEvent); // Close handle for event
                        return dwErr;
                    }
                    else
                    {
                        // Read completed successfully.
                        sprintf_s(strOutputDebugBuffer,  MAX_PATH, "fnReceiveThreadIdle: Detected Char: %c %d\n", cRead, dwRead);
                        OutputDebugString(strOutputDebugBuffer);
                        
                        // Read char on port
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
                            CloseHandle(stReceive->hCommPort); // Delete this line
                            CloseHandle(ov.hEvent); // Close handle for event
                            stReceive->bStopped = true;
                            return 0;
                        }
                    }

                    //  Reset flag so that another opertion can be issued.
                    bWaitRead = FALSE;
                    break;

                case WAIT_TIMEOUT:
                    // Operation isn't complete yet. fWaitingOnRead flag isn't
                    // changed since I'll loop back around, and I don't want
                    // to issue another read until the first one finishes.
                    //
                    // This is a good time to do some background work.
                    OutputDebugString("fnReceiveThreadIdle: WaitForSingleObject Timed out\n");
                    break;

                default:
                    // Error in the WaitForSingleObject; abort.
                    // This indicates a problem with the OVERLAPPED structure's
                    // event handle.
                    break;
            }
        }
    }

    // for debugging
    // comment out this part
    CloseHandle(stReceive->hCommPort);
    CloseHandle(ov.hEvent); // Close handle for event
    stReceive->bStopped = true;
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
    char  cRead[1024 + 1] = {0};
    char  strOutputDebugBuffer[2048] = {0};
    char  strErrorBuffer[MAX_PATH+1] = {0};

    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    // stop the transmit thread
    (stReceive->pTransmit)->bStopped = true;
    // sendData(ACK)
    OutputDebugString("fnReceiveThreadActive: Started\n");

    // return value for wait
    BOOL bWaitReturn = false;
    BOOL bWaitRead = false;
    DWORD dwWait;
    DWORD dwErr;

    // end of test

    // SetCommMask to wait for ACK
    if(!SetCommMask(stReceive->hCommPort, EV_RXCHAR))
    {
        // Error setting communications event mask.
        dwErr = GetLastError();
        sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadActive: Error SetCommMask: 0x%x\n", dwErr);
        OutputDebugString(strErrorBuffer);
        return dwErr;
    }

    // Create overlapped structure
    OVERLAPPED ov;
    ov.hEvent = CreateEvent(NULL,   // default security attributes
                            TRUE,   // manual-reset event
                            FALSE,  // not signaled
                            NULL);   // no name

    // Initialize the rest of the OVERLAPPED structure to zero.
    ov.Internal = 0;
    ov.InternalHigh = 0;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    BOOL bWaitCommEvent = false;

    while(true)
    {
        // Stop block
        if(stReceive->bRequestStop == true)
        {
            //stReceive->bStopped = true; // let idle handle the stop
            CloseHandle(ov.hEvent); // Close handle for event
            return 0;
        }

        // Wait for event from the commport
        if (!bWaitRead)
        {
            // Issue read operation.
            if (!ReadFile(stReceive->hCommPort, &cRead, 10, &dwRead, &ov)) // Wait for 10 Bytes for now
            {
                dwErr = GetLastError();
                if (dwErr != ERROR_IO_PENDING)     // read not delayed?
                {
                    sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadActive: Error ReadFile: 0x%x\n", dwErr);
                    OutputDebugString(strErrorBuffer);
                    CloseHandle(ov.hEvent); // Close handle for event
                    //stReceive->bStopped = true;
                    stReceive->bActive = false;
                    return dwErr;
                }
                else
                {
                    bWaitRead = TRUE;
                }
            }
            else 
            {
                // Should not be able to go here
            }
        } // if (!bWaitRead)

        if (bWaitRead) 
        {
            dwWait = WaitForSingleObject(ov.hEvent, 10000); // Change timeout and put it somewhere
            if (dwWait == WAIT_OBJECT_0)
            {
                if (!GetOverlappedResult(stReceive->hCommPort, &ov, &dwRead, FALSE))
                {
                    dwErr = GetLastError();
                    sprintf_s(strErrorBuffer,  MAX_PATH, "fnReceiveThreadActive: Error GetOverlappedResult: 0x%x\n", dwErr);
                    OutputDebugString(strErrorBuffer);
                    CloseHandle(ov.hEvent); // Close handle for event
                    
                    //stReceive->bStopped = true;
                    stReceive->bActive = false;
                    return dwErr;
                }
                else
                {
                    // Read completed successfully.
                    sprintf_s(strOutputDebugBuffer,  MAX_PATH, "fnReceiveThreadActive: String: %s %d\n", cRead, dwRead);
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

                //  Reset flag so that another opertion can be issued.
                bWaitRead = FALSE;
            }
            else if (dwWait == WAIT_TIMEOUT)
            {
                // Operation isn't complete yet. fWaitingOnRead flag isn't
                // changed since I'll loop back around, and I don't want
                // to issue another read until the first one finishes.
                //
                // This is a good time to do some background work.
                OutputDebugString("fnReceiveThreadActive: WaitForSingleObject Timed out\n");
                CloseHandle(ov.hEvent); // Close handle for event

                //stReceive->bStopped = true;
                stReceive->bActive = false;

                return 0;
            }
            else
            {
                // Error in the WaitForSingleObject; abort.
                // This indicates a problem with the OVERLAPPED structure's
                // event handle.
                break;
            }
        }
    }

    // for debugging
    // comment out this part
    CloseHandle(ov.hEvent); // Close handle for event

    //stReceive->bStopped = true;
    stReceive->bActive = false;

    return 0;
} // End of fnReceiveThreadActive