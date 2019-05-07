#include "atomic_ops.h"
#include "hrtimer_x86.c"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sched.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

int NUM_THREADS;
int cuenta = 0;
long lock = 0;
ticket_lock_t tlock;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;

//2 4 8 hilos cuenta++ 50000 veces
void *noSincronismo(void *arg) {
    for (int i = 0; i < 50000; i++) {
        cuenta ++;
    }
}

void *mtx(void *arg) {
    for (int i = 0; i < 50000; i++) {
        pthread_mutex_lock(&mtx1);
        cuenta++;
        pthread_mutex_unlock(&mtx1);  
    }
}

void tas_lock(void) {
    while (tas(&lock) == 1);
}

void tas_unlock(void) {
    lock = 0;
}

void *TAS(void *arg) {
   for (int i = 0; i < 50000; i++) {
       tas_lock();
       cuenta++;
       tas_unlock();
   } 
}

void tastas_lock(void) {
    do {
        while (lock == 1);
    } while (tas(&lock) == 1);
}

void tastas_unlock(void) {
    lock = 0;
}

void *TASTAS(void *arg) {
    for (int i = 0; i < 50000; i++) {
        tastas_lock();
        cuenta++;
        tastas_unlock();
    }
}

void tastasb_lock(void) {
    int delay = 1;
    do {
        while (lock == 1) {
            backoff(&delay);
        }
    } while (tas(&lock) == 1); 
}

void tastasb_unlock(void) {
    lock = 0;
}

void *TASTASBACKOFF() {
    for (int i = 0; i < 50000; i++) {
        tastasb_lock();
        cuenta++;
        tastasb_unlock();
    }
}

void ticket_lock(void) {
    ticket_acquire(&tlock);
}

void ticket_unlock(void) {
   ticket_release(&tlock); 
}

void *TICKET(void *arg) {
    for (int i = 0; i < 50000; i++) {
        ticket_lock();
        cuenta++;
        ticket_unlock();
    }
}

int main(void) {
    printf("Cantidad de threads: ");
    scanf("%d", &NUM_THREADS);
    printf("\n");
    pthread_t threads[NUM_THREADS];
    
    // SIN SINCRONISMO 
    double start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, noSincronismo, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    double end = gethrtime_x86();
    
    printf("Tiempo sin sincronismo: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
    
    // PTHREAD LOCKS
    cuenta = 0;
    start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, mtx, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    end = gethrtime_x86();
    
    printf("Tiempo con pthread locks: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
     
    // TAS
    cuenta = 0;
    start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, TAS, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    end = gethrtime_x86();
    
    printf("Tiempo con TAS: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
    
    // TASTAS
    cuenta = 0;
    start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, TASTAS, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    end = gethrtime_x86();
    
    printf("Tiempo con TASTAS: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
    
    // TASTAS CON BACKOFF
    cuenta = 0;
    start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, TASTASBACKOFF, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    end = gethrtime_x86();
    
    printf("Tiempo con TASTAS con backoff: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
    
    // TICKET
    cuenta = 0;
    start = gethrtime_x86();

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, TICKET, (void*) t);
    }

    for (long t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    end = gethrtime_x86();
    
    printf("Tiempo con TICKET: %f\n", end - start); 
    printf("Cuenta = %d\n\n", cuenta);
    
    pthread_exit(NULL);

    return 0;
}
