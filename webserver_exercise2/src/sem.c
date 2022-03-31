#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

typedef struct SEM {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SEM;

SEM *sem_init(int initVal) {
    SEM *sem = malloc(sizeof(SEM));

    if (pthread_mutex_init(&sem->mutex, NULL)) {
        free(sem);
        perror("Something went wrong in initializing the mutex.");
        return NULL;
    }
    if (pthread_cond_init(&sem->cond, NULL)) {
        free(sem);
        pthread_mutex_destroy(&sem->mutex);
        perror("Something went wrong in initializing the cond.");
        return NULL;
    }

    sem->value = initVal;
    return sem;
}

int sem_del(SEM *sem) {
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    free(sem);
    return 0;
}

void P(SEM *sem) {

    pthread_mutex_lock(&(sem->mutex));

    while(sem->value==0) {
        pthread_cond_wait(&(sem->cond), &(sem->mutex));
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

