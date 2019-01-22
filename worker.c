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
    printf("Hello from worker %d\n", myNodeIdx);

    // TODO move this to scratchpad region
    char workerBuffer[MSG_BUF_LEN];
    int workerBufferLen;


    // TESTING: start the chain reaction
    if (myNodeIdx == 0) {
        // wait for dest node to receive
        sleep(1);

        // fill the buffer...
        const char *str = "Hello.";
        strncpy(workerBuffer, str, MSG_BUF_LEN);
        workerBufferLen = strlen(str) + 1;
        workerBuffer[workerBufferLen] = 0;  // null-terminate

        // ...and send the data.
        sendData(1, workerBuffer, workerBufferLen);
    }

    while (true) {
        recv(myNodeIdx, workerBuffer, &workerBufferLen);
        printf("Received %d bytes.\n", workerBufferLen);
    }

    return NULL;
}
