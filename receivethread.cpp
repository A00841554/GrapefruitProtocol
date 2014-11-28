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

    DWORD dwRead;
    DWORD dwErr;
    DWORD dwWait;
    BOOL bWaitReturn = false;
    BOOL bWaitRead = false;

    char  cRead;
    char strOutputDebugBuffer[MAX_PATH+1] = {0};
    char strErrorBuffer[MAX_PATH+1] = {0};

    // Set receive structure
    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    OutputDebugString("fnReceiveThreadIdle: Started\n");

    while(true)
    {
        if(stReceive->bRequestStop == TRUE)
        {
            stReceive->bStopped = TRUE;
            return 0;
        }

        // Wait for event from the commport
        if (!bWaitRead)
        {
            // Wait for ENQ
            if (!ReadFile(*(stReceive->pHCommPort), &cRead, 1, &dwRead, stReceive->pOverlapped))
            {
                dwErr = GetLastError();
                if (dwErr != ERROR_IO_PENDING)     // read not delayed?
                {
                    sprintf_s(strErrorBuffer,
                              MAX_PATH,
                              "fnReceiveThreadIdle: Error ReadFile: 0x%x\n",
                              dwErr);
                    OutputDebugString(strErrorBuffer);
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
                    if((stReceive->pTransmit)->bActive == TRUE)
                    {
                        stReceive->bStopped = TRUE;
                    }
                    else
                    {
                        stReceive->bActive = TRUE;
                        fnReceiveThreadActive(stReceive);
                    }
                    return 0;
                }
            }
        }

        if (bWaitRead) 
        {
            switch(WaitForSingleObject(stReceive->pOverlapped->hEvent, 500)) // Change timeout and put it somewhere
            {
                // Read completed.
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(*(stReceive->pHCommPort), stReceive->pOverlapped, &dwRead, false))
                    {
                        dwErr = GetLastError();
                        sprintf_s(strErrorBuffer,
                                  MAX_PATH,
                                  "fnReceiveThreadIdle:" 
                                  "Error GetOverlappedResult: 0x%x\n",
                                  dwErr);
                        OutputDebugString(strErrorBuffer);
                        return dwErr;
                    }
                    else
                    {
                        // Read completed successfully.
                        sprintf_s(strOutputDebugBuffer,
                                  MAX_PATH,
                                  "fnReceiveThreadIdle: Detected Char: %c %d\n",
                                  cRead,
                                  dwRead);
                        OutputDebugString(strOutputDebugBuffer);
                        
                        // Read char on port
                        // Check if chRead is an ENQ
                        //if(cRead == 5)
                        if(cRead == 48) // press 0 to test
                        {
                            if((stReceive->pTransmit)->bActive == TRUE)
                            {
                                stReceive->bStopped = TRUE;
                            }
                            else
                            {
                                stReceive->bActive = TRUE;
                                fnReceiveThreadActive(stReceive);
                            }
                            stReceive->bStopped = TRUE;
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

    DWORD dwRead;
    char  cRead[1024 + 1] = {0};
    char  strOutputDebugBuffer[2048] = {0};
    char  strErrorBuffer[MAX_PATH+1] = {0};

    ReceiveArgs * stReceive = (ReceiveArgs*) lpArg;

    // stop the transmit thread
    (stReceive->pTransmit)->bStopped = TRUE;
    // sendData(ACK)
    OutputDebugString("fnReceiveThreadActive: Started\n");

    // return value for wait
    BOOL bWaitReturn = FALSE;
    BOOL bWaitRead   = FALSE;
    DWORD dwWait;
    DWORD dwErr;

    while(true)
    {
        // Stop block
        if(stReceive->bRequestStop == TRUE)
        {
            stReceive->bStopped = true; // let idle handle the stop
            return 0;
        }

        // Wait for event from the commport
        if (!bWaitRead)
        {
            // Issue read operation.
            if (!ReadFile(*(stReceive->pHCommPort), &cRead, 10, &dwRead, stReceive->pOverlapped)) // Wait for 10 Bytes for now
            {
                dwErr = GetLastError();
                if (dwErr != ERROR_IO_PENDING)     // read not delayed?
                {
                    sprintf_s(strErrorBuffer,
                              MAX_PATH,
                              "fnReceiveThreadActive: Error ReadFile: 0x%x\n",
                              dwErr);
                    OutputDebugString(strErrorBuffer);
                    
                    stReceive->bStopped = true;
                    stReceive->bActive = FALSE;
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
            dwWait = WaitForSingleObject(stReceive->pOverlapped->hEvent, 10000); // Change timeout and put it somewhere
            if (dwWait == WAIT_OBJECT_0)
            {
                if (!GetOverlappedResult((*stReceive->pHCommPort),
                                        stReceive->pOverlapped,
                                        &dwRead,
                                        FALSE))
                {
                    dwErr = GetLastError();
                    sprintf_s(strErrorBuffer,
                              MAX_PATH,
                              "fnReceiveThreadActive:"
                              "Error GetOverlappedResult: 0x%x\n",
                              dwErr);
                    OutputDebugString(strErrorBuffer);
                    
                    stReceive->bStopped = true;
                    stReceive->bActive = false;
                    return dwErr;
                }
                else
                {
                    // Read completed successfully.
                    sprintf_s(strOutputDebugBuffer,
                              MAX_PATH,
                              "fnReceiveThreadActive: String: %s %d\n",
                              cRead,
                              dwRead);
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
                OutputDebugString("fnReceiveThreadActive:" 
                                  "WaitForSingleObject Timed out\n");
                CloseHandle(stReceive->pOverlapped->hEvent); // Close handle for event

                stReceive->bStopped = true;
                stReceive->bActive = FALSE;

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

    stReceive->bStopped = true;
    stReceive->bActive = FALSE;

    return 0;
} // End of fnReceiveThreadActive