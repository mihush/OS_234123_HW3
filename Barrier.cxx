/****************** INCLUDES *****************/
#include "Barrier.h"


/****************** IMPLEMENTATION *****************/
/*** Constructor ***/
Barrier::Barrier(unsigned int num) {
    /* Lock to protect the counter that counts the number of threads at the barrier */
    sem_init(&global_lock , 0 , 1);

    /* Lock used to wait for all the threads to arrive */
    sem_init(&threads_lock , 0 , 0);

    /* Lock used to wait for all the threads to execute the critical code */
    sem_init(&threads_lock2 , 0 , 1);
    counter = 0;
    num_of_threads = num;
}

void Barrier::wait() {
    sem_wait(&global_lock);
    ++counter;
    sem_post(&global_lock);

    /* First wait - wait for all the threads to arrive */
    if (counter == num_of_threads) {
        /* Lock the second lock to prevent a thread from passing and entering wait again */
        sem_wait(&threads_lock2);
        /* Unlock the first lock because we want the threads to free each other 1 by 1 */
        sem_post(&threads_lock);
    }
    /* Continue to free each other 1 by 1 */
    sem_wait(&threads_lock);
    sem_post(&threads_lock);

    /* Second wait - wait for all the threads to finish exceuting the critical code.
       This makes the barrier reusable */
    sem_wait(&global_lock);
    --counter;
    if (counter == 0) {
        /* Lock the first lock to prevent a thread from passing and entering wait again */
        sem_wait(&threads_lock);
        /* Unlock the second lock because we want the threads to free each other 1 by 1 */
        sem_post(&threads_lock2);
    }
    sem_post(&global_lock);

    /* Continue to free each other 1 by 1 */
    sem_wait(&threads_lock2);
    sem_post(&threads_lock2);
}

Barrier::~Barrier() {
    sem_destroy(&threads_lock);
    sem_destroy(&threads_lock2);
    sem_destroy(&global_lock);
}