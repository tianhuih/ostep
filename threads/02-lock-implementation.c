/**
 * @brief method 1 - controlling interrupts
 * many disadvantages:
 *  1. need to trust the program using the lock
 *  2. lost of interrupts
 *  3. does not work for multiprocessors
 */

void lock_1() {
    DisableInterrupts();
}

void unlock_1() {
    EnableInterrupts();
}


/**
 * @brief method 2 - single flag
 * If a thread calls lock() while another thread is in critical section
 * it will spin-wait in the while loop for it to unclock (clear the flag)
 *
 * 1. This implementation is incorrect because:  if thread 2 calls lock()
 *          and interrupts when thread 1 just exited the while loop in lock()
 *          and did not set the flag yet - both threads will have the lock
 * 2. Performance issues with spin-wait. Wasting CPU time
 */

typedef struct __lock_t_single_flag {
    int flag;   // 0 if available, 1 if held
}lock_t_single_flag;

void init(lock_t_single_flag *mutex) {
    mutex->flag = 0;
}

void lock(lock_t_single_flag *mutex) {
    while (mutex->flag == 1)    // test the flag
        ;                       // spin-wait (do nothing)
    mutex->flag = 1;
}

void unlock(lock_t_single_flag *mutex) {
    mutex->flag = 0;
}



/**
 * @brief method 3 - spin lock with test-and-set
 * with hardware support TestAndSet
 * benifit: correctness, prevents the problem in method 2.
 * drawback:
 *      1. need preemptive scheduler to make sense on single CPUs
 *      2. it is not fair and may lead to starvation
 */

// store 'new' into old pointer and return the old value
// hardware support for atomic exchange
int TestAndSet(int *old_ptr, int new) {
    int old = *old_ptr;
    *old_ptr = new;
    return old;
}

typedef struct __lock_t_test_set {
    int flag;
} lock_t_test_set;

void init(lock_t_test_set *lock) {
    lock->flag = 0;
}

// using the atomic operating TestAndSet, thie implementation
// avoids the problem in method 2 (single flag) - whenever a
// thread find flag = 0, it sets it to 1 in one atomic action
void lock(lock_t_test_set *lock) {
    while (TestAndSet(&lock->flag, 1) == 1)
        ;       // spin-wait, do nothing
}

void unlock(lock_t_test_set *lock) {
    lock->flag = 0;
}


/**
 * @brief method 4 - compare and swap
 *
 */

// if value at pointer location is expected, set it to net
// otherwise do nothing
// always return the old value at pointer
int CompareAndSwap(int *ptr, int expected, int new) {
    int old = *ptr;
    if (old == expected) {
        *ptr = new;
    }
    return old;
}

void lock(lock_t_single_flag *lock) {
    while (CompareAndSwap(&lock->flag, 0, 1) == 1) ; // spin
}

/**
 * @brief method 5: load linked and store-conditional
 */

// fetches a value from memory and places it in a register
int LoadLinked(int *ptr) {
    return *ptr;
}

int StoreConditional(int *ptr, int value) {
    if (no update to *ptr since LoadLinked to this address) {
        *ptr = value;
        return 1;
    } else {
        return 0;   // fail to update
    }
}

void init(lock_t_single_flag *lock) {
    lock->flag = 0;
}

void lock(lock_t_single_flag *lock) {
    while (1) {
        while (LoadLinked(&lock->flag) == 1)
            ;
        // if set to 1 was a success, return
        // otherwise, start over
        if (StoreConditional(&lock->flag, 1) == 1)
            return;
    }
}

// another way to write the lock function
void lock_concise(lock_t_single_flag *lock) {
    while (LoadLinked(&lock->flag) ||
            !StoreConditional(&lock->flag, 1))
        ;   // spin
}

void unlock(lock_t_single_flag *lock) {
    lock->flag = 0;
}

/**
 * @brief method 6: fetch and add
 */

// increment value at ptr address by one and return old value
int FetchAndAdd(int *ptr) {
    int old = *ptr;
    *ptr = old + 1;
    return old;
}

typedef struct __lock_ticket_t {
    int turn;
    int ticktet;
} lock_ticke_t;


void init(lock_ticke_t *lock) {
    lock->ticktet = 0;
    lock->turn = 0;
}

void lock(lock_ticke_t *lock) {
    int currTicket = FetchAndAdd(&lock->ticktet);
    while (lock->turn != currTicket)
        ;   // spin
}

void unlock(lock_ticke_t *lock) {
    lock->turn = lock->turn + 1;
}

