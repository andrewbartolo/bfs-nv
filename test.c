#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "broker.h"


typedef struct {
    int workerID;
} workerCtrl;



// Main entry point for the pthreads for the worker.
void *circularTest(void *_wc) {
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



/*
 * This function prints the name of the current test being performed,
 * kicks off the workers, kicks off the master message broker, and then
 * reaps the workers when they're done, indicating whether the test
 * passed or failed.
 */
int assertPass(const char *testName, void *(*testImpl)(void *)) {
    printf("Beginning %s...", testName);

    pthread_t handles[N_WORKERS];
    workerCtrl ctrls[N_WORKERS];

    // fill out the worker ctrls and kick off the workers
    for (int i = 0; i < N_WORKERS; ++i) {
        ctrls[i].workerID = i;

        if (pthread_create(&handles[i], NULL, testImpl, (void *)&ctrls[i])) {
            fprintf(stderr, "ERROR: could not create pthread.\n");
            return -1;
        }
    }

    // to see if any workers need to transfer memory amongst themselves
    broker_loop();

    // join the threads (though currently just runs forever)
    for (int i = 0; i < N_WORKERS; ++i) {
        if (pthread_join(handles[i], NULL)) {
            fprintf(stderr, "ERORR: could not join pthread.\n");
            return -1;
        }
    }

    printf("\t[\033[0;32mPASS\033[0;0m]\n");
    return 0;
}

int main(int argc, char *argv[]) {
    /*
     * The main testing loop. Runs a bunch of unit tests and asserts that
     * they all pass.
     */
    printf("\t[\033[0;32mPASS\033[0;0m]\n");
    int retVal = assertPass("circularTest", circularTest);


    return retVal;
}
