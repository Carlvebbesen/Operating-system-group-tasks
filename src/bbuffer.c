#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
#include "bbuffer.h"


typedef struct BNDBUF {
    int *buffer;
    SEM *pointerSync;
    SEM *freeSlotsSync;
    SEM *fullSlotsSync;
    size_t head;
    size_t tail;
} BNDBUF;

BNDBUF bndbuf;

BNDBUF *bb_init(unsigned int size) {

    int newBuffer[size];

    bndbuf.buffer = malloc(size * sizeof(int));
    memcpy(bndbuf.buffer, newBuffer, size * sizeof(int));
    bndbuf.pointerSync = sem_init(1);
    bndbuf.freeSlotsSync = sem_init(size);
    bndbuf.fullSlotsSync = sem_init(0);
    bndbuf.head = 0;
    bndbuf.tail = 0;

    return &bndbuf;
}

void bb_add(BNDBUF *bb, int fd) {
    P(bb->freeSlotsSync);
    P(bb->pointerSync);

    bb->buffer[bb->head] = fd;
    ++bb->head;
    if (bb->head >= sizeof(bb->buffer))
    {
        bb->head = 0;
    }

    V(bb->pointerSync);
    V(bb->fullSlotsSync);
}

int bb_get(BNDBUF *bb)
{
    P(bb->fullSlotsSync);
    P(bb->pointerSync);

    int fd = bb->buffer[bb->tail];
    bb->buffer[bb->tail] = 0;
    ++bb->tail;
    if (bb->tail >= sizeof(bb->buffer)) {
        bb->tail = 0;
    }

    V(bb->pointerSync);
    V(bb->freeSlotsSync);

    return fd;
}
