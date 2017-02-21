#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// global parameters
#define ARRAY_SIZE 400
#define NTHREADS 32
// global variables
int * gArray;
int globalSum = 0;

void * sumArray(void * ip)
{

  // all variables declared in sumArray are local to each thread
  int * i = (int *) ip; // cast type of input to (int *)
  int threadArraySliceSize = ARRAY_SIZE / NTHREADS;
  int startIndex = *i * threadArraySliceSize; // unique value for each thread
  int endIndex = (*i + 1) * threadArraySliceSize; // unique value for each thread
  int sumSlice = 0; // initialize a sum that is local to each thread

  int j;
  for ( j = startIndex; j < endIndex; j++ ) {
     sumSlice += gArray[j];
     globalSum += gArray[j]; // WARNING! globalSum is shared but being modified simultaenously! Race condition!
  }
  printf("Hi.  I'm thread %d, sumSlice = %d, avg Value = %5.1f\n", *i, sumSlice, (float) sumSlice / (float) threadArraySliceSize );

  pthread_exit(NULL);

}

void fillOutArray()
{

   time_t t;
   srand((unsigned) time(&t)); // initialize random number generator
   int i;
   for ( i=0; i < ARRAY_SIZE ; i++) {
      gArray[i] = rand() % 1000; // random int between 1 and 1000
   }

}

int main()
{

  int i, vals[NTHREADS];
  pthread_t threads[NTHREADS];
  void * retval;
  gArray = malloc(ARRAY_SIZE*sizeof(int));  // already declared with global scope (see above), here we allocate space for array

  // fill out array with random values
  // Note: gArray and ARRAY_SIZE are both global, so
  //       there is no need to pass these to fillOutArray
  fillOutArray(); 

  for (i = 0; i < NTHREADS; i++) {
    vals[i] = i;
    int rc = pthread_create(&threads[i], NULL, sumArray,(void *) &vals[i]);
    if (rc) {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
  }

  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], &retval);
  }

  int correctSum = 0;
  for (i = 0; i < ARRAY_SIZE; i++) {
    correctSum += gArray[i];
  }
  printf("globalSum: %d, correct answer: %d\n",globalSum,correctSum);

  free(gArray);
  pthread_exit(NULL);

}