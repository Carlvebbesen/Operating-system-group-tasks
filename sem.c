#include <pthread.h>
#include <stdio.h>
#include "sem.h"

struct SEM {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct SEM sem;

SEM *sem_init(int initVal) {

    if (pthread_mutex_init(&(sem.mutex), NULL)) {
        perror("Something went wrong in initializing the mutex.");
        return NULL;
    }
    if (pthread_cond_init(&(sem.cond), NULL)) {
        perror("Something went wrong in initializing the cond.");
        return NULL;
    }

    sem.value = initVal;
    return &sem;
}

int sem_del(SEM *sem) {
    //TODO
}

void P(SEM *sem) {

    pthread_mutex_lock(&(sem->mutex));

    while(sem->value==0) {
        phtead_cond_wait(&(sem->cond), &(sem->mutex));
    }
    --sem->value;

    pthread_mutex_unlock(&(sem->mutex));
}

void V(SEM *sem) {

    pthread_mutex_lock(&(sem->mutex));

    ++sem->value;
    pthread_cond_signal(&(sem->cond));

    pthread_mutex_unlock(&(sem->mutex));
}

