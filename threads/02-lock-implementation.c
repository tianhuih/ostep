// method 1 - controlling interrupts
// many disadvantages:
// 1. need to trust the program using the lock
// 2. lost of interrupts
// 3. does not work for multiprocessors

void lock_1() {
    DisableInterrupts();
}

void unlock_1() {
    EnableInterrupts();
}



// method 2 - spin lock with test-and-set (hardware support)

