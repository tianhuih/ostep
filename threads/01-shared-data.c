#include <stdio.h>
#include <pthread.h>

// an example of race condition when working with shared data
// due to uncontrolled scheduling

// sample output:
// main: begin (counter = 0)
// A: begin
// B: begin
// A: done
// B: done
// main: done with both (counter = 10219538)

// the result varies from run to run
// this is called an indeterminate program


static volatile int counter = 0;

void *mythread(void *arg) {
    printf("%s: begin\n", (char *) arg);
    int i;
    for (i = 0; i < 1e7; i++) {
        // a critical section - code that access the same variable
        // needs mutual exclusion
        counter = counter + 1;
    }
    printf("%s: done\n", (char *) arg);
    return NULL;
}

// launches two threads and waits for them to complete
int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("main: begin (counter = %d)\n", counter);
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");

    // waits for the threads to complete
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: done with both (counter = %d)\n", counter);
    return 0;
}