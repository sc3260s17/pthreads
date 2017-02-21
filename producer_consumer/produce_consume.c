#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// global parameters
#define NTHREADS 4
// global (shared) variables
int * gArray;
int dataAvail = 0;
int prodDataCnt = 0;
int consDataCnt = 0;
pthread_mutex_t dataMutex;
pthread_cond_t dataCond;

void addData()
{
  gArray[prodDataCnt] = rand() % 100; // random int between 1 and 100
  printf("Produced data %d\n",gArray[prodDataCnt]);
}

void * produce(void * ip)
{

  int * i = (int *) ip; // cast type of input to (int *)
  printf("Hi.  I'm thread %d, ready to produce\n", *i);

  while ( prodDataCnt < NTHREADS-1 ) {
    pthread_mutex_lock(&dataMutex);    
    addData();
    prodDataCnt++;
    dataAvail = 1;
    pthread_cond_signal(&dataCond);
    pthread_mutex_unlock(&dataMutex);   
  } 

  pthread_exit(NULL);

}

void * consume(void * ip)
{

  int * i = (int *) ip; // cast type of input to (int *)

  pthread_mutex_lock(&dataMutex);
  printf("Hi.  I'm thread %d, ready to consume. consDataCnt: %d\n", *i, consDataCnt);
  while ( !dataAvail ) {
    pthread_cond_wait(&dataCond,&dataMutex); // lock released while waiting
  }

  int consIndex = consDataCnt;
  consDataCnt++;
  if ( consDataCnt == prodDataCnt ) {
    dataAvail = 0;
  }
  pthread_mutex_unlock(&dataMutex);

  // This is where the consumer would actually do work
  // do_work()
  printf("Thread %d Consuming data %d. consDataCnt: %d\n",*i,gArray[consIndex],consDataCnt);

  pthread_exit(NULL);

}

void fillOutArray()
{

  int i;
  for ( i=0; i < NTHREADS-1 ; i++) {
     gArray[i] = 0;
  }

}

int main()
{

  int i, vals[NTHREADS];
  pthread_t threads[NTHREADS];
  void * retval;

  pthread_mutex_init( &dataMutex, NULL );
  pthread_cond_init( &dataCond, NULL );

  gArray = malloc((NTHREADS-1)*sizeof(int));  // already declared with global scope (see above), here we allocate space for array
  time_t t;
  srand((unsigned) time(&t)); // initialize random number generator

  // initialize array with zeros
  fillOutArray(); 

  for (i = 0; i < NTHREADS; i++) {
    vals[i] = i;
    int rc;
    if ( i == 0 ) {
       rc = pthread_create(&threads[i], NULL, produce,(void *) &vals[i]);
    }
    else {
       rc = pthread_create(&threads[i], NULL, consume,(void *) &vals[i]);
    }
    if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
  }

  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], &retval);
  }

  free(gArray);
  pthread_mutex_destroy( &dataMutex );
  pthread_cond_destroy( &dataCond );
  pthread_exit(NULL);

}