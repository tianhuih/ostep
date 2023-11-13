// a classic concurrency problem

// five philosopher sitting around a table
// five forks between each philosopher
// a philosopher might think and does not eat
// when they eat, they need two forks, one from
// left and one from right

// that is, each philosopher is a unique thread
// doing the following:
//
// while (1) {
//     think();
//     get_forks(p);
//     eat();
//     put_forks(p);
// }

// the challenge is to write the routines get_forks()
// and put_forks() that ensures high concurrency without
// deadlocks or starvations

#include <semaphore.h>

#define N 5         // number of philosophers

int left(int p)     { return p; }
int right(int p)    { return (p + 1) % N; }

sem_t forks[N];

// this is a broken solution that causes
// deadlock - each philosipher might hold
// a fork and waiting for another fork
// [0 p] [1 p] [2 p] [3 p] [4 p]
void get_forks_broken(int p) {
    sem_wait(&forks[left(p)]);
    sem_wait(&forks[right(p)]);
}

// this solution works and breaks the dependency
// by changing the behavior of at least one
// philosopher (in our case, no.4)
void get_forks(int p) {
    if (p == 4) {
        sem_wait(&forks[right(p)]);
        sem_wait(&forks[left(p)]);
    } else {
        sem_wait(&forks[left(p)]);
        sem_wait(&forks[right(p)]);
    }
}

void put_forks(int p) {
    sem_post(&forks[left(p)]);
    sem_post(&forks[right(p)]);
}
