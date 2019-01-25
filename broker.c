#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "broker.h"
#include "util.h"


typedef enum msgType {
    DATA_FOR_ME,
    WAKEUP_FOR_ME,
    SHUTDOWN_FOR_ME,
    RECVING,
    IDLE
} msgType;


/*
 * These represent the registers exported by the master FPGA.
 * TODO make sure these are set and read atomically!!
 */
static bool RTS[N_WORKERS];    // Ready to Send
static bool CTS[N_WORKERS];    // Clear to Send
static bool RTR[N_WORKERS];    // Ready to Receive
static bool CTR[N_WORKERS];    // Clear to Receive


/*
 * The all-important buffer through which data is transferred from
 * worker -> FPGA -> worker. Eventually, this will be realized in hardware.
 * There's only one of these, so it's the (broker's and worker's combined?)
 * responsibility to mediate access to it correctly.
 */
static char brokerBuffer[MSG_BUF_LEN];
static int bufferLen;
static int destNodeIdx;

/*
 * Synchronous, blocking send.
 */
void sendData(int myNodeIdx, int _destNodeIdx, char *workerBuffer, int _bufferLen) {
    assert(!RTS[myNodeIdx]);    // Flag should be false at the start

    // Set the flag TODO atomically, indicate to master we want to send
    RTS[myNodeIdx] = true;

    // Synchronize on the FPGA's ACK (FPGA sets CTS)
    spinWhile(!CTS[myNodeIdx]);

    // Actually put data over the pins to the FPGA master
    // TODO do this in hardware
    memcpy(brokerBuffer, workerBuffer, _bufferLen);
    bufferLen = _bufferLen;
    destNodeIdx = _destNodeIdx;

    // Worker unsets RTS[i] to indicate to FPGA that it's sent all data
    RTS[myNodeIdx] = false;

    // FPGA's ACK of receiver's receipt is clearing the sender's CTS flag
    spinWhile(CTS[myNodeIdx]);
}

// TODO implement wakeup and shutdown later
void sendWakeup(int _destNodeIdx) {

}
void sendShutdown(int _destNodeIdx) {

}

/*
 * Synchronous, blocking receive.
 */
void recv(int myNodeIdx, char *workerBuffer, int *_bufferLen) {
    assert(!RTR[myNodeIdx]);    // Flag should be false at the start

    // Set the flag TODO atomically, indicate to master we're ready to receive
    RTR[myNodeIdx] = true;

    // Synchronize on the FPGA's ACK (FPGA sets CTR)
    spinWhile(!CTR[myNodeIdx]);

    // Actually pull the data out of the FPGA master
    // TODO do this in hardware
    memcpy(workerBuffer, brokerBuffer, bufferLen);
    *_bufferLen = bufferLen;

    // Tell the FPGA we're done DMAing data out
    RTR[myNodeIdx] = false;

    // Here, FPGA's ACK of receiver's receipt is receiver clearing its own CTR
    spinWhile(CTR[myNodeIdx]);
}




void broker_loop() {
    printf("Beginning broker loop\n");

    while (true) {
        // Walk the RTS bitvector, seeing if anyone wants to send
        // TODO replace with e.g. hardware interrupts instead of polling
        for (int senderNodeIdx = 0; senderNodeIdx < N_WORKERS; ++senderNodeIdx) {
            if (RTS[senderNodeIdx]) {
                assert(!CTS[senderNodeIdx]);    // shouldn't already be set
                CTS[senderNodeIdx] = true;      // TODO make atomic
                // At this step, worker DMAs into our buffer
                spinWhile(RTS[senderNodeIdx]);
                spinWhile(!RTR[destNodeIdx]);   // wait for receiver node ready
                assert(!CTR[destNodeIdx]);      // should't already be set
                CTR[destNodeIdx] = true;        // tell receiver to DMA from us now
                spinWhile(RTR[destNodeIdx]);    // wait for receiver done w/DMA

                CTS[senderNodeIdx] = false;     // release the sender
                CTR[destNodeIdx] = false;       // release the receiver
            }
        }



    }
}
