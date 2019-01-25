#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "broker.h"
#include "worker.h"




// Main entry point for the pthreads for the worker.
void *worker(void *_wc) {
    workerCtrl *wc = (workerCtrl *)_wc;
    int myNodeIdx = wc->workerID;
    printf("Worker %d starting up.\n", myNodeIdx);

    // TODO move this to scratchpad region
    char workerBuffer[MSG_BUF_LEN];
    int workerBufferLen;


    // TESTING: start the chain reaction
    if (myNodeIdx == 0) {
        // fill the buffer...
        const char *str = "Hello.";
        strncpy(workerBuffer, str, MSG_BUF_LEN);
        workerBufferLen = strlen(str) + 1;
        workerBuffer[workerBufferLen] = 0;  // null-terminate

        // ...and send the data.
        sendData(0 /* Our ID, 0 */, 1 /* Next worker ID, 1 */, workerBuffer,
                 workerBufferLen);
    }

    while (true) {
        recv(myNodeIdx, workerBuffer, &workerBufferLen);
        printf("Worker %d received %d bytes: %s\n", myNodeIdx, workerBufferLen,
                workerBuffer);

        // forward to the next worker.
        sendData(myNodeIdx, (myNodeIdx + 1) % N_WORKERS, workerBuffer, workerBufferLen);
    }

    return NULL;
}
