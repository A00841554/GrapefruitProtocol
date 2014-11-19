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
 * @signature  void fnReceiveThreadIdle(ReceiveArgs * stReceive)
 *
 * @param      stReceive - Structure that holds the flags for receivethread
 *
 * @return     returns void
 *
 * @note       note1
 *
 */
void fnReceiveThreadIdle(ReceiveArgs * stReceive) {
    // TO DO readIdle
    // Clear receive buffer

    while(true) {
        if(stReceive->bRequestStop == true){
            stReceive->bStopped = true;
            return;
        }

        //Wait for event from the receive buffer
        // WaitCommEvent here
        //if(waiting times-out) {
        //    continue
        //}
        //if(event from receive buffer arrives) {
        //    if ENQ received {
        //        if receive.transmit.active == true {
        //            receive.stopped = true
        //        } else {
        //            receive.active = true
        //            Call ReceiveActive(receive)
        //        }
        //        return
        //    }
        //}
    } // End of while
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
 * @signature  void fnReceiveThreadActive(ReceiveArgs * stReceive)
 *
 * @param      stReceive - Structure that holds the flags for receivethread
 *
 * @return     returns void
 *
 * @note       note1
 *
 */
void fnReceiveThreadActive(ReceiveArgs * stReceive) {
    // TO DO readactive
    // stop the transmit thread
    (stReceive->pTransmit)->bStopped;
    // sendData(ACK)

    while(true) {
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
            if(stReceive->bRVI == false) {
                //sendData(ACK)
            }

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
    }// End of while loop

    stReceive->bStopped = true;
    stReceive->bActive = false;
} // End of fnReceiveThreadActive