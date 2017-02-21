#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// global parameters
#define NTHREADS 4

typedef struct {
   int id;
   float data;
} argList;

void * barrierFunc(void * ip)
{

  argList * args = (argList *) ip;
  int myid = args->id;
  float mydata = args->data;
  printf("Hello, I am thread %d with data %6.3f\n",myid,mydata);

  pthread_exit(NULL);

}

int main()
{

  int i;
  argList myArgs[NTHREADS];
  pthread_t threads[NTHREADS];
  void * retval;

  for (i = 0; i < NTHREADS; i++) {
    myArgs[i].id = i;
    myArgs[i].data = (float)(i+1) * 23.423;
    int rc = pthread_create(&threads[i], NULL, barrierFunc,(void *) &myArgs[i]);
    if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
  }

  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], &retval);
  }

  pthread_exit(NULL);

}