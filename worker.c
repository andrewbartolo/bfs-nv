#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "broker.h"
#include "worker.h"




// Main entry point for the pthreads for the worker.
void *worker(void *_wc) {
    workerCtrl *wc = (workerCtrl *)_wc;
    printf("Hello from worker %d\n", wc->workerID);

    // TODO move this to scratchpad region
    //char workerBuffer[MSG_BUF_LEN];

    printf("Allocated worker buffer on stack.\n");

    return NULL;
}
