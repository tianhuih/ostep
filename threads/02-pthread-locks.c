#include <pthread.h>
#include <stdio.h>

// an example of using pthread from POSIX library
int main(int argc, char *argv[]) {
    int balance = 0;

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lock);
    balance = balance + 5;
    pthread_mutex_unlock(&lock);
    printf("%d\n", balance);
    return 0;
}