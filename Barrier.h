#ifndef BARRIER_H_
#define BARRIER_H_

#include <semaphore.h>

class Barrier {
public:
	Barrier(unsigned int num_of_threads);
	void wait();
	~Barrier();

protected:
	/* Lock to protect the counter that counts the number of threads at the barrier */
	sem_t global_lock;

	/* Lock used to wait for all the threads to arrive */
	sem_t threads_lock;

	/* Lock used to wait for all the threads to execute the critical code */
	sem_t threads_lock2;
	/* Counter - current number of threads at the barrier */
	int counter;

	/* Number of threads */
	int num_of_threads;
};

#endif // BARRIER_H_

