#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "atomic_ops.h"
#include "hrtimer_x86.c"

long NUM_THREADS;
pthread_barrier_t barrier;
double start, end;

struct bar_type {
	long counter;
	pthread_mutex_t lock;
	long flag;
} bar_name;

void *bar(void *p) {
	for (long i = 0; i < (long)p; i++) {
		pthread_barrier_wait(&barrier);
	}
}

void *BARRIER(void *p) {
	pthread_mutex_lock(&bar_name.lock);
	if (bar_name.counter == 0)
		bar_name.flag = 0;
	long mycount = bar_name.counter++;
	pthread_mutex_unlock(&bar_name.lock);
	if (mycount == (long)p) {
		bar_name.counter = 0;
		bar_name.flag = 1;
	} else 
		while (bar_name.flag == 0);
}

void *REVERSE_BARRIER(void *p) {
	long local_sense = !(local_sense);
	pthread_mutex_lock(&bar_name.lock);
	long mycount = bar_name.counter++;
	if (bar_name.counter == (long)p) {
		pthread_mutex_unlock(&bar_name.lock);
		bar_name.counter = 0;
		bar_name.flag = local_sense;
	} else {
		pthread_mutex_unlock(&bar_name.lock);
		while (bar_name.flag != local_sense);
	}
}

int main()
{
	printf("Cantidad de threads: ");
    scanf("%ld", &NUM_THREADS);
    printf("\n");
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	long vueltas = 10;	
	pthread_t threads[NUM_THREADS];

    start = gethrtime_x86();
	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_create(&threads[t], NULL, bar, (void*)vueltas);
	}

	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL);
	}
	end = gethrtime_x86();
	printf("Tiempo barrera pthread: %f", end - start);
	
	printf("\n");
	pthread_barrier_destroy(&barrier);
    
	// Barrier reverse
	start = gethrtime_x86();
	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_create(&threads[t], NULL, REVERSE_BARRIER, (void*)vueltas);
	}

	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL);
	}
	end = gethrtime_x86();
	printf("Tiempo barrera reverse: %f", end - start);
	
	printf("\n");
	pthread_exit(NULL);	
/*
	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_create(&threads[t], NULL, BARRIER, &t);
	}

	for (long t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL);
	}
*/	
	return 0;
}
