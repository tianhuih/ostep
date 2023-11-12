#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>

/**
 * @brief semaphore basic operations
 */

int main(int argc, char *argv[]) {

    // a semaphore is an object with an integer value
    // that we can manipulate with two routines
    sem_t s;

    // initializes the semaphore with value 1;
    // the second argument indicates that the semaphore
    // is shared between threads in the same process.
    sem_init(&s, 0, 1);

    // decrement the value of semaphore s by one
    // wait if value of semaphore s is negative
    // the value (when nagative) = num of waiting threads
    sem_wait(&s);

    // increment the value of semaphore s by one
    // if there are one or more threads waiting, wake one
    sem_post(&s);

    return 0;
}

/**
 * @brief binary semaphore - used as a lock
 */

void semAsLock(void *arg) {
    sem_t m;
    // initialize the value of sem to 1
    sem_init(&m, 0, 1);

    sem_wait(&m);
    // critical section here
    sem_post(&m);
}

/**
 * @brief semaphore used for ordering
 *
 * similar to the use of condition variable, one thread
 * waits for something to happen while another thread makes
 * that something happen and then signalling it
 */

sem_t s;

void *child(void *arg) {
    printf("child\n");
    sem_post(&s);
    return NULL;
}

void *parent(void *arg) {
    // set the initial value to 0
    // if child runs first, value = 1 when parent runs sem_wait()
    // so it will return immediately
    // if parent runs first, sem_wait() will wait until child
    // calles sem_post() and finishes running.
    sem_init(&s, 0, 0);
    printf("parent begin\n");
    pthread_t p;
    pthread_create(&p, NULL, child, NULL);
    sem_wait(&s);
    printf("parent ended\n");
    return NULL;
}



