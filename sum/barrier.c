#include "barrier.h"
#include <stdio.h>

//*********************************************************************
//  barrier_init - Initialize a barrier data structure
//*********************************************************************

void barrier_init(Barrier_t *b, int target)
{
  pthread_mutex_init(&(b->lock), NULL);
  pthread_cond_init(&(b->cond), NULL);
  
  b->target = target;
  b->count = 0;
  b->exit_count = 0;
}

//*********************************************************************
// barrier - Blocks until all threads reach the barrier
//*********************************************************************

void barrier(Barrier_t *b)
{
   //Need to lock the data so others don't clobber what I am
   //doing.  Don't forget to unlock once we are done.
   pthread_mutex_lock(&(b->lock));

  b->count++;
  if (b->count < b->target) {
     do {      //** Loop for false positives
        pthread_cond_wait(&(b->cond), &(b->lock));
     } while (b->exit_count == 0);    
  } else {
    b->count = 0;  //Reset the barrier for the next round
    b->exit_count = b->target;   //** Also set the exit countdown for false positive triggers
    pthread_cond_broadcast(&(b->cond));  //Wake everyone else up
  }
  b->exit_count--;  //** Decrement the exit count so it gets reset to 0 when all threads exit
  pthread_mutex_unlock(&(b->lock));
}

