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


    // Run for 100 rounds, say
    int sendsRemaining = 1, recvsRemaining = 1;
    if (myNodeIdx == 0) --recvsRemaining; // Worker 0 doens't perform an initial recv
    if (myNodeIdx == N_WORKERS - 1) --sendsRemaining; // Last worker doesn't perform a final send



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
        --sendsRemaining;
    }

    while (true) {
        if (recvsRemaining == 0) break;
        recv(myNodeIdx, workerBuffer, &workerBufferLen);
        --recvsRemaining;

        printf("Worker %d received %d bytes: %s\n", myNodeIdx, workerBufferLen,
                workerBuffer);

        if (sendsRemaining == 0) break;
        sendData(myNodeIdx, (myNodeIdx + 1) % N_WORKERS, workerBuffer, workerBufferLen);
        --sendsRemaining;
    }

    setWorkerDone(myNodeIdx);
    return NULL;
}



/*
 * This function prints the name of the current test being performed,
 * kicks off the workers, kicks off the master message broker, and then
 * reaps the workers when they're done, indicating whether the test
 * passed or failed.
 */
int assertPass(const char *testName, void *(*testImpl)(void *)) {
    printf("Beginning %s...\n", testName);

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

    printf("%s:\t\t[\033[0;32mPASS\033[0;0m]\n", testName);
    return 0;
}

int main(int argc, char *argv[]) {
    /*
     * The main testing loop. Runs a bunch of unit tests and asserts that
     * they all pass.
     */
    int retVal = assertPass("circularTest", circularTest);


    return retVal;
}
