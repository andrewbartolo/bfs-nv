#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "broker.h"
#include "worker.h"



/*
 * For now, this serves as the main system driver. In the future, tests
 * will be driven by unittest.c or similar.
 */
int driver() {
    printf("Kicking off multi-node BFS simulation\n");

    pthread_t handles[N_WORKERS];
    workerCtrl ctrls[N_WORKERS];

    // fill out the worker ctrls and kick off the workers
    for (int i = 0; i < N_WORKERS; ++i) {
        ctrls[i].workerID = i;

        if (pthread_create(&handles[i], NULL, worker, (void *)&ctrls[i])) {
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

    // Shouldn't reach here
    return 0;
}

int main(int argc, char *argv[]) {
    int retVal = driver();

    return retVal;
}
