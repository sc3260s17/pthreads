#include <pthread.h>

typedef struct {
  pthread_mutex_t lock;         //*** The actual lock
  pthread_cond_t  cond;         //** Condition trigger
  int             target;       //** Number of threads to hit the barrier each time
  int             count;        //** Current number of threads at the barrier
  int             exit_count;   //** Used to detect false triggers.  It's actually a "countdown" of threads exiting the barrier
} Barrier_t;

void barrier_init(Barrier_t *b, int target);
void barrier(Barrier_t *b);
