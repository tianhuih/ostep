#include <pthread.h>

// a basic implementation of a semaphore

typedef struct __mysem_t {
    int val;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} mysem_t;

void mysem_init(mysem_t *sem, int value) {
    sem->val = value;
    Mutex_init(&sem->mutex);
    Cond_init(&sem->cond);
}

void mysem_wait(mysem_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    sem->val--;
    while (sem->val < 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    pthread_mutex_unlock(&sem->mutex);
}

void mysem_post(mysem_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    sem->val++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}

