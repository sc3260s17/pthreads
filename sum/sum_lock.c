#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int count;
int blocksize;
double sum;
int max_val;
pthread_mutex_t lock;

void *thread_sum(void *arg)
{
   int * iptr = (int *)arg;
   int myid = *iptr;
   int mywork;
   int start, end, i;
   double mysum;

   printf("thread_sum: start myid=%d count=%d\n", myid, count);

   mywork = 0;
   pthread_mutex_lock(&lock);
   while (count <= max_val) {
       //** Set my range and inc count
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
   pthread_mutex_unlock(&lock);

   printf("thread_sum: end myid=%d mywork=%d\n", myid, mywork);
   
   return(NULL);
}


//*************************************************************************
//*************************************************************************

int main(int argc, char **argv)
{
  int nthreads;
  double sum2;
  int i;
  int * myid;
  pthread_t * threads;
  void * dummy;

  if (argc != 4) {
     printf("Usage: ./sum_lock nthreads blocksize max_val\n");
     printf("\n");
     return(0);
  }

  i = 1;
  nthreads = atoi(argv[i]); 
  i++;
  blocksize = atoi(argv[i]); 
  i++;
  max_val = atol(argv[i]); 
  i++;

  printf("nthreads = %d\nmax_val = %d\n", nthreads, max_val);

  //** Allocate the space for the threads
  threads = (pthread_t *)malloc(sizeof(pthread_t)*nthreads);
  assert(threads != NULL);
  myid = (int *)malloc(sizeof(int)*nthreads);
  assert(myid != NULL);
  
  pthread_mutex_init(&lock, NULL);

  count = 0;
  sum = 0;

  //** Launch the threads
  for (i=0; i<nthreads; i++) {
     myid[i] = i;
     int rc = pthread_create( &threads[i], NULL, thread_sum, (void *) &myid[i] );
     if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }

  }

  //** Wait for them to complete
  for (i=0; i<nthreads; i++) {
     pthread_join(threads[i], &dummy);
  }

  pthread_mutex_destroy(&lock);

  sum2 = max_val;
  sum2 = 0.5 * sum2 * (sum2+1);
  printf("Calculated Sum = %lf  should be %lf\n", sum, sum2);

  pthread_exit(NULL);

}

