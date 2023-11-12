#include <pthread.h>

/**
 * @brief an example of covering conditions
 * For memory allocation and free.
 *
 * Problem: If thread1 wants to allocate 100 bytes,
 * thread2 wants to allocate 10 bytes, and thread3
 * frees 50 bytes, which thread should it wake up?
 *
 * Solution1: use pthread_cond_broadcast() instead of
 *      pthread_cond_signal() to wake up all threads.
 *      This is called a covering condition, as it covers
 *      all cases where a thread should wake up.
 *
 */

#define MAX_HEAP_SIZE 1024

int bytesLeft = MAX_HEAP_SIZE; // number of free heap bytes

pthread_cond_t c;
pthread_mutex_t m;

void *allocate(int size) {
    pthread_mutex_lock(&m);
    while (bytesLeft < size)
        pthread_cond_wait(&c, &m);
    void *ptr = ...;    // get mem from heap
    bytesLeft -= size;
    pthread_mutex_unlock(&m);
    return ptr;
}

void free(void*ptr, int size) {
    pthread_mutex_lock(&m);
    bytesLeft += size;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}