#include <semaphore.h>
#include <stdio.h>

/**
 * @brief semaphore for producer/consumer problem
 *
 */


#define MAX 1024

int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;

void put(int val) {
    buffer[fill_ptr] = val;
    fill_ptr = (fill_ptr + 1) % MAX;
}

int get() {
    int temp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    return temp;
}

sem_t fill, empty;
int loops;

// This is not working because two producer threads
// might write to the same buffer position!
void *producer_v1(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty);
        put(i);
        sem_post(&fill);
    }
}

void *consumer_v2(void *args) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&fill);
        int temp = get();
        sem_post(&empty);
        printf("%d\n", temp);
    }
}

sem_t mutex;

// This causes a deadlock (imagine a consumer having the mutex
// but waiting for the fill signal, while the producer waiting
// for the mutex and unable to fill)
void *producer_deadlock(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&mutex);       // added line
        sem_wait(&empty);
        put(i);
        sem_post(&fill);
        sem_post(&mutex);       // added line
    }
}

void *consumer_deadlock(void *args) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&mutex);       // added line
        sem_wait(fill);
        int temp = get();
        sem_post(empty);
        sem_post(&mutex);       // added line
        printf("%d\n", temp);

    }
}

// this version is working
void *producer_correct(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);       // added line
        put(i);
        sem_post(&mutex);       // added line
        sem_post(&fill);

    }
}

void *consumer_correct(void *args) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(fill);
        sem_wait(&mutex);       // added line
        int temp = get();
        sem_post(&mutex);       // added line
        sem_post(empty);
        printf("%d\n", temp);

    }
}


int main(int argc, char *argv[]) {
    // consider the initialization value of semaphores
    // as the number of resources can be given away immediately
    sem_init(&empty, 0, MAX);
    sem_init(&fill, 0, 0);

    // adding a mutex
    sem_init(&mutex, 0, 1);
    // ...
}
