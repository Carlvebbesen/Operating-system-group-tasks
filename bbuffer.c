#include <stdio.h>
#include <stdbool.h>
#include <sem.h>
#include "bbuffer.h"


struct BNDBUF {
    int buffer[1];
    SEM *pointerSync;
    SEM *freeSlotsSync;
    SEM *fullSlotsSync;
    size_t head;
    size_t tail;
    unsigned int size;
};

struct BNDBUF bndbuf;

BNDBUF *bb_init(unsigned int *size) {

    bndbuf.pointerSync = sem_init(1);
    bndbuf.freeSlotsSync = sem_init(size);
    bndbuf.fullSlotsSync = sem_init(0);
    bndbuf.head = 0;
    bndbuf.tail = 0;

    return &bndbuf;
}

int bb_add(BNDBUF *bb, int fd) {
    P(bb->freeSlotsSync);
    P(bb->pointerSync);

    bb->buffer[bb->head] = fd;
    ++bb->head;
    if (bb->head >= bb->size) {
        bb->head = 0;
    }

    V(bb->pointerSync);
    V(bb->fullSlotsSync);
}

int bb_get(BNDBUF *bb) {
    P(bb->fullSlotsSync);
    P(bb->pointerSync);

    int fd = bb->buffer[bb->tail];
    bb->buffer[bb->tail] = 0;
    ++bb->tail;
    if (bb->tail >= 4) {
        bb->tail = 0;
    }

    V(bb->pointerSync);
    V(bb->freeSlotsSync);

    return fd;
}
