#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_WORKERS 8

typedef struct {
    int workerID;
} workerCtrl;


void *worker(void *_wc) {
    workerCtrl *wc = (workerCtrl *)_wc;
    printf("Hello from worker %d\n", wc->workerID);
    while (1) {
        printf("%d\n", wc->workerID);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("Kicking off multi-node BFS simulation\n");

    pthread_t handles[N_WORKERS];
    workerCtrl ctrls[N_WORKERS];

    // fill out the worker ctrls and kick off the threads
    for (int i = 0; i < N_WORKERS; ++i) {
        ctrls[i].workerID = i;

        if (pthread_create(&handles[i], NULL, worker, (void *)&ctrls[i])) {
            fprintf(stderr, "ERROR: could not create pthread.\n");
            return 1;
        }
    }


    // join the threads
    for (int i = 0; i < N_WORKERS; ++i) {
        if (pthread_join(handles[i], NULL)) {
            fprintf(stderr, "ERORR: could not join pthread.\n");
            return 1;
        }
    }

    return 0;
}
