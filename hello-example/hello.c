/*
 *  * hello.c -- spawns four threads that print their ids
 *   */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void * printme(void * ip)
{

  // here we are re-casting the type of ip 
  // to point to an int, and then assigning that
  // integer address to i
  int * i = (int *) ip;
  printf("Hi.  I'm thread %d\n", *i);
  pthread_exit(NULL);

}

int main()
{

  int i, vals[4];
  pthread_t threads[4];
  void *retval;

  // at this point there is only one master thread
  for (i = 0; i < 4; i++) {
    vals[i] = i;
    // fork worker threads to execute printme().
    // pthread_create() is non-blocking, which means
    // the master thread will call this and immediately
    // proceed to the next loop iteration without waiting
    // for thread i to complete printme().
    int rc = pthread_create(&threads[i], NULL, printme,(void *) &vals[i]);
    if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
  }

  // The master thread waits for
  // each of the worker threads to complete.
  for (i = 0; i < 4; i++) {
    printf("Trying to join with tid %d\n", i);
    pthread_join(threads[i], &retval);
    printf("Joined with tid %d\n", i);
  }

  pthread_exit(NULL);

}