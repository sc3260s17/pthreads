#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// global parameters
#define NTHREADS 4
pthread_barrier_t tBarrier;

void * barrierFunc(void * ip)
{

  int * i = (int *) ip;

  printf("Thread %d arrived at barrier\n",*i);
  pthread_barrier_wait(&tBarrier);
  printf("Thread %d past barrier\n",*i);

  pthread_exit(NULL);

}

int main()
{

  int i, vals[NTHREADS];
  pthread_t threads[NTHREADS];
  void * retval;
  
  pthread_barrier_init(&tBarrier,NULL,NTHREADS);

  for (i = 0; i < NTHREADS; i++) {
    vals[i] = i;
    int rc = pthread_create(&threads[i], NULL, barrierFunc,(void *) &vals[i]);
    if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
  }

  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], &retval);
  }

  pthread_barrier_destroy(&tBarrier);
  pthread_exit(NULL);

}