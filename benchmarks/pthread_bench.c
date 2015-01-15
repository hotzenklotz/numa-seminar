/*
The MIT License (MIT)

Copyright (c) 2014 Erick Elejalde & Leo Ferres
{eelejalde|lferres}@udec.cl

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//
#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

/* helper macros */
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

/* Constants */

#define CACHE_ALIGN 64
#ifndef NUMA_NODES
#define NUMA_NODES 2
#endif

/* Numa constants */
enum numastate {GLOBAL_RELEASE = 1, LOCAL_RELEASE, BUSY};
#define MAX_VISITORS 64

/* helper functions */
static inline int getcpu(int *cpu, int *node) {
#ifdef SYS_getcpu
  int status;
  status = syscall(SYS_getcpu, cpu, node, NULL);
  *node = *cpu % 2 ? 1 : 0;
  return (status == -1) ? status : *cpu;
#else
  return -1; // unavailable
#endif
}

/* Delete this function before production */
unsigned long gettid() {
  pthread_t ptid = pthread_self();
  unsigned long threadId = 0;
  memcpy(&threadId, &ptid, min(sizeof(threadId), sizeof(ptid)));
  return threadId;
}

/******************************************************************************/
/* RWBench Benchmark */
/******************************************************************************/
/*
  wprob: probability to enter the CS in read-write mode
  shared_array: 64 elements integer array shared between all threads
  l: ReadWrite Lock
  WCSLen: time to be elapsed in the critical section when in read-write mode
  RCSLen: time to be elapsed in the critical section when in read-only mode
  RCSLen: time to be elapsed in the non-critical section
*/

double mtimediff(struct timeval te, struct timeval ts) {
 return ((te.tv_sec - ts.tv_sec) * 1000000L +
		(te.tv_usec - ts.tv_usec)) / 1000;
}

struct opts {
  int wcslen;
  int rcslen;
  int ncslen;
  int prob;
  int *array;
  pthread_rwlock_t lock;
  unsigned msecs;
};

void *rw_bnchmrk(void *arg) {

  /* struct unpack */
  struct opts o = *(struct opts*)arg;
  int wprob = o.prob;
  int *shared_array  = o.array;
  pthread_rwlock_t l = o.lock;
  int wcslen = o.wcslen;
  int rcslen = o.rcslen;
  int ncslen = o.ncslen;
  unsigned msecs = o.msecs;

  int node, private_array[64];
  long i = 0; /* result */

  struct timeval tsmain, tfmain, tsncs, tfncs, tswcs, tfwcs, tsrcs, tfrcs;

  int seed = time(NULL);

  gettimeofday(&tsmain, NULL);
  gettimeofday(&tfmain, NULL);
  while (mtimediff(tfmain, tsmain) <= msecs) {
	i++;

	gettimeofday(&tsncs, NULL);
	gettimeofday(&tfncs, NULL);
	while (mtimediff(tfncs, tsncs) <= ncslen) {
	  int r = rand_r(&seed) % 500;
	  private_array[rand_r(&seed) % 64] += r;
	  private_array[rand_r(&seed) % 64] -= r;
	  gettimeofday(&tfncs, NULL);
	}

	int p = rand_r(&seed) % 100;

	if( p < wprob) {

	  gettimeofday(&tswcs, NULL);
	  //rw_acquire(l, W, &node); /*entering the critical section*/
	  pthread_rwlock_wrlock(&l);

	  gettimeofday(&tfwcs, NULL);
	  while (mtimediff(tfwcs, tswcs) <= wcslen) {
	int r = rand_r(&seed) % 500;
	shared_array[rand_r(&seed) % 64] += r;
	shared_array[rand_r(&seed) % 64] -= r;
	gettimeofday(&tfwcs, NULL);
	  }

	  pthread_rwlock_unlock(&l);
	} else {
	  gettimeofday(&tsrcs, NULL);
	  pthread_rwlock_rdlock(&l);
	  //rw_acquire(l, R, &node); /*entering the critical section*/
	  gettimeofday(&tfrcs, NULL);
	  while (mtimediff(tfrcs,tsrcs) <= rcslen) {
	volatile int i1 = shared_array[rand_r(&seed) % 64];
	volatile int i2 = shared_array[rand_r(&seed) % 64];
	gettimeofday(&tfrcs, NULL);
	  }

	  pthread_rwlock_unlock(&l);
	}
	gettimeofday(&tfmain, NULL);
  }

  return (void *)i;
}

/* main */
int main(int argc, char *argv[])
{
  int threads = atoi(argv[5]);

  /* initialize rw lock */
  pthread_rwlock_t l;


  /* populate struct */
  struct opts *o = malloc(sizeof(struct opts));
  o->wcslen = atoi(argv[1]);
  o->rcslen = atoi(argv[2]);
  o->ncslen = atoi(argv[3]);
  o->prob = atoi(argv[4]);

  o->array = calloc(64, sizeof(int));
  o->msecs = atoi(argv[7]);


  int rc = pthread_rwlock_init(&l, NULL);
  o->lock = l;


  pthread_t *t = malloc(sizeof(pthread_t) * threads);
  for (int i = 0; i < threads; i++)
	pthread_create(&t[i], NULL, rw_bnchmrk, (void *)o);

  long ac = 0;
  void *status;

  for (int i = 0; i < threads; i++) {
	pthread_join(t[i], &status);
	ac += (long)status;
  }

  printf ("%ld\n",ac);

  pthread_rwlock_destroy(&o->lock);
  free(t);
  free(o->array);
  free(o);

  return 0;
}
