#pragma once

typedef struct {
    int workerID;
} workerCtrl;

void *worker(void *_wc);
