#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "barrier.h"

int count;
int blocksize;
int shutdown;
double sum;
int max_val;
pthread_mutex_t lock;
pthread_cond_t server_cond;
Barrier_t bar;

void do_sum(int myid)
{
   int mywork;
   int start, end, i;
   double mysum;

   printf("do_sum: start myid=%d count=%d\n", myid, count);

   mywork = 0;
   pthread_mutex_lock(&lock);
   while (count <= max_val) {
       //** Set my range and increment count
       start = count;
       count = count + blocksize;
       end = count;       
       pthread_mutex_unlock(&lock);

       if (end > max_val) end = max_val+1;

       //** Do partial sum
       mysum = 0;
       for (i=start; i<end; i++) {
          mywork++;
          mysum = mysum + i;
       }

       //** Add partial sum to global
       pthread_mutex_lock(&lock);
       sum = sum + mysum;
       pthread_mutex_unlock(&lock);

       //** reset lock for main loop
       pthread_mutex_lock(&lock);
   }
   printf("do_sum: end myid=%d mywork=%d sum=%lf\n", myid, mywork, sum);
   pthread_mutex_unlock(&lock);
   
   return;
}

void *thread_sum(void *arg)
{
   int *iptr = (int *)arg;
   int myid = *iptr;

   printf("thread_sum: start myid=%d count=%d\n", myid, count);

   pthread_mutex_lock(&lock);
   while (shutdown == 0) {
       pthread_mutex_unlock(&lock);

       do_sum(myid);

       barrier(&bar);  //** This barrier syncs everybody after the sum

       if (myid == 0) {   //** Send wakeup to main
          pthread_mutex_lock(&lock); 
          pthread_cond_signal(&server_cond);
          pthread_mutex_unlock(&lock); 
       }

       barrier(&bar);  //** This barrier pauses everybody until the server updates everything

       pthread_mutex_lock(&lock);
   }
   pthread_mutex_unlock(&lock);

   printf("thread_sum: end myid=%d sum=%lf\n", myid, sum);
   
   return(NULL);
}


//*************************************************************************
//*************************************************************************

int main(int argc, char **argv)
{

  int nthreads;
  double sum2;
  int i,n;
  int *myid;
  pthread_t *threads;
  void *dummy;

  if (argc < 4) {
     printf("Usage: ./sum_cond nthreads blocksize max_val1 max_val2 ....\n");
     printf("\n");
     return(0);
  }

  n = 1;
  nthreads = atoi(argv[n]); n++;
  blocksize = atoi(argv[n]); n++;
  barrier_init(&bar, nthreads);

  printf("nthreads = %d  *  max_val = %d\n", nthreads, max_val);

  //** Allocate the space for the threads
  threads = (pthread_t *)malloc(sizeof(pthread_t)*nthreads);
  assert(threads != NULL);
  myid = (int *)malloc(sizeof(int)*nthreads);
  assert(myid != NULL);
  
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&server_cond, NULL);

  shutdown = 0;

  pthread_mutex_lock(&lock);  //** Get the lock before launching the threads
  
  //** Launch the threads
  for (i=0; i<nthreads; i++) {
     myid[i] = i;
     pthread_create(&(threads[i]), NULL, thread_sum, (void *)&(myid[i]));
  }

  //** Main loop
  for (i=n; i<argc; i++) {
      //** Update the max_val and init the count/sum
      max_val = atol(argv[i]);
      count = 0;
      sum = 0; 

      pthread_cond_wait(&server_cond, &lock);  //** Wait until finished

      //** Print the results
      sum2 = max_val;
      sum2 = 0.5 * sum2 * (sum2+1);
      printf("Calculated Sum = %lf  should be %lf\n", sum, sum2);
  }

  //** Trigger shutdown
  shutdown = 1;
  pthread_mutex_unlock(&lock);

  //** Wait for them to complete
  for (i=0; i<nthreads; i++) {
     pthread_join(threads[i], &dummy);
  }

  pthread_mutex_destroy(&lock);

  return 1;

}

