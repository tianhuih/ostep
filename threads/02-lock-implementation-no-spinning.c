/**
 * @brief a lock with queues, test-and-set, yield, and wakeup
 * with OS support - par() and unpark() primitives on Solaris
 */

typedef struct __lock_t {
    int flag;
    int guard;      // use guard to safeguard operations on q and flag
    queue_t *q;     // a queue of threads waiting to be executed
} lock_t;

void lock_init(lock_t *m) {
    m->flag = 0;
    m->guard = 0;
    queue_init(m->q);
}

void lock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ;       // acquire guard lock by spinning
    if (m->flag == 0) {
        m->flag = 1;    // lock is acquired
        m->guard = 0;   // release guard
    } else {
        queue_add(m->q, gettid());  // add current thread to queue
        m->guard = 0;   // release guard (before yield!)
        park();         // yield
    }
}

// caution: wakeup-waiting race
// if thread 1 interrupts after thread 2 releases guard and before park(), and thread 1 releases the lock, then thread 2 can sleep forever!
// solution 1: use setpark(); system call before releasing guard.
// solution 2: pass the guard into the kernel

void unlock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ;       // acquire guard lock by spinning
    if (queue_empty(m->q)) {
        m->flag = 0;     // release lock
    } else {
        unpark(queue_remvoe(m->q));
                        // hold lock for next thread in queue
    }
    m->guard = 0;       // release guard
}


/**
 * @brief a lock with queues, test-and-set, yield, and wakeup
 * with OS support - futex on Linux
 * each futex has a specific physical memory location and a
 * in-kernel queue.
 */

void mutex_lock(int *mutex) {
    int v;
    // got the mutex in fastpath
    // cleared bit 31 (the highest bit of the integer)
    if (atomic_bit_test_set(mutex, 31) == 0)
        return;
    // track number of waiters on the lock with all other bits
    atomic_increment(mutex);
    while (1) {
        if (atomic_bit_test_set(mutex, 31) == 0) {
            atomic_decrement(mutex);
            return;
        }
        // the lock is held if it is negative (highest bit = 1)
        // otherwise it is not held, we can try again
        v = *mutex;
        if (v >= 0)
            continue;
        // futex_wait(address, expected)
        // sleep if value at address is expected
        // otherwise return immediately
        futex_wait(mutex, v);
    }
}

void mutex_unlock(int *mutex) {
    if (atomic_add_zero(mutex, 0x80000000))
        return;
    futex_wake(mutex);
}