#include <pthread.h>
#include <stdio.h>

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() {
    pthread_mutex_lock(&m);
    done = 1;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

void thr_join() {
    pthread_mutex_lock(&m);
    // The variable done is necessary because it attends to
    // the case where the child thread runs first, signals on
    // the condition (there is nothing to wake up), and then the
    // parent's join runs. If there is no done variable, the parent
    // could stuck as the child's signal has already been fired!
    while (done == 0) {
        // Atomically releases the lock and put the calling
        // thread to sleep; when the thread wakes up (upon
        // a signal), the lock is re-acquired and returned
        // to the caller
        // Prevents the sleep/wakeup race condition as in
        // 02-lock-implementation-no-spinning
        pthread_cond_wait(&c, &m);
    }
    pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    thr_exit();
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent begin\n");
    pthread_t p;
    pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent end\n");
    return 0;
}