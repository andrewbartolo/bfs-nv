#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "broker.h"


/*
 * interrupts[i] indicates the operation being performed for *destination*
 * worker i, by some other source worker. (TODO: We don't need to keep track of
 * the source worker?)
 */
typedef enum msgType {
    DATA_FOR_ME,
    WAKEUP_FOR_ME,
    SHUTDOWN_FOR_ME,
    RECVING,
    IDLE
} msgType;
static msgType interrupts[N_WORKERS];



/*
 * The all-important buffer through which data is transferred from
 * worker -> FPGA -> worker. Eventually, this will be realized in hardware.
 * There's only one of these, so it's the (broker's and worker's combined?)
 * responsibility to mediate access to it correctly.
 */
static char brokerBuffer[MSG_BUF_LEN];
static int bufferLen;

// TODO send and recv should assume an asynchronous middle man (and not
// do all the work themselves)

void sendData(int destNodeIdx, char *workerBuffer, int _bufferLen) {
    /*
     * If the receiver isn't ready, the message will be missed (NOT buffered).
     */
    //assert(interrupts[destNodeIdx] == RECVING);
    if (interrupts[destNodeIdx] != RECVING) {
        printf("\033[0;33mWarning: intended receiver node wasn't ready yet.\033[0m\n");
    }
    // TODO if the receiver isn't ready, just block (spin)
    while (interrupts[destNodeIdx] != RECVING) { }

    // perform the memcpy() into the FPGA buffer
    memcpy(brokerBuffer, workerBuffer, _bufferLen);
    bufferLen = _bufferLen;

    // set the interrupt so that the worker knows its data is ready to be
    // retrieved. TODO do this in hardware

    interrupts[destNodeIdx] = DATA_FOR_ME;
}

// TODO implement wakeup and shutdown later
void sendWakeup(int destNodeIdx) {

}
void sendShutdown(int destNodeIdx) {

}

// polls the master FPGA continuously for any messages it might have
// received, and returns a void * to the buffer where the message is placed.
//
// currently spins, but we could also use a mutex
void recv(int myNodeIdx, char *workerBuffer, int *_bufferLen) {   // blocking recv
    interrupts[myNodeIdx] = RECVING;

    while (interrupts[myNodeIdx] != DATA_FOR_ME) { } // spin.


    memcpy(workerBuffer, brokerBuffer, bufferLen);
    *_bufferLen = bufferLen;

    // clear the interrupts
    interrupts[myNodeIdx] = IDLE;
}




void broker_loop() {
    printf("Beginning broker loop\n");
    while (true) {



        sleep(1);
    }
}
