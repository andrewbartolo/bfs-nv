#pragma once

#include "worker.h"

#define N_WORKERS 8
#define MSG_BUF_LEN 256


/*
 * These four functions are the worker-facing part of the broker interface.
 * They define how the chips communicate with each other, using the FPGA as
 * message broker.
 *
 * These functions will need to be adjusted to port over to the actual board.
 */
void sendData(int nodeIdx, char *workerBuffer, int _bufferLen);
void sendWakeup(int nodeIdx);
void sendShutdown(int nodeIdx);
void recv(int myNodeIdx, char *workerBuffer, int *_bufferLen);   // blocking recv




// checks if any of the workers have messages to send;
void broker_loop();
