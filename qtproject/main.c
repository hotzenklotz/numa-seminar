#include <stdio.h>
#include <main.h>
#include <rw_bo_msc.h>

/*
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static my_pthread_rwlock_t thread_array[100000];
static int index = 0; */


/* Acquire write lock for RWLOCK.  */
int pthread_rwlock_wrlock (my_pthread_rwlock_t *rwlock)
{
  printf("CUSTOM WRITE LOCK ACQUIRED\n");
  //my_pthread_rwlock_t *myRWLock = findLock(rwlock);
  rw_acquire(rwlock->numaLock, W, &rwlock->node);
  return 0;
}

int pthread_rwlock_unlock (my_pthread_rwlock_t *rwlock)
{
    printf("CUSTOM RW UNLOCK\n");
	//my_pthread_rwlock_t *myRWLock = findLock(rwlock);
	rw_release(rwlock->numaLock, R, rwlock->node);
    return 0;
}

int pthread_rwlock_rdlock (my_pthread_rwlock_t *rwlock)
{
  printf("CUSTOM READ LOCK ACQUIRED\n");
  //my_pthread_rwlock_t *myRWLock = findLock(rwlock);
  rw_acquire(rwlock->numaLock, R, &rwlock->node);
  return 0;
}

int pthread_rwlock_destroy(my_pthread_rwlock_t *rwlock)
{
	//my_pthread_rwlock_t *myRWLock = findLock(rwlock);
	destroy_rw(rwlock->numaLock);
	return 0;
}

/* every original pthread is the 'key' for our numa-aware lock. Do a look-up. */
/*struct rw *findLock(pthread_rwlock_t *rwlock)
{
    struct rw *result;
    for (int i = 0; i < index; i++)
    {
        if (rwlock == thread_array[i].originalLock)
        {
            result = thread_array[i].numaLock;
        }
    }
    return result;
}*/

typedef int pthread_rwlockattr_t; // Bitte bei Gelegenheit an das Original aus pthread.h anpassen
int pthread_rwlock_init(my_pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
   //my_pthread_rwlock_t *myRWLock = malloc(sizeof(my_pthread_rwlock_t));
   rwlock->numaLock = init_rw(READER); //malloc(sizeof(rw));
   rwlock->node = 0;

   // thread_array[index] = *myRWLock;

   //increment the alookup-array index in a thread-safe manner
//   int rc = pthread_mutex_lock(&mutex);
//   index++;
//   rc = pthread_mutex_unlock(&mutex);

  return 0;
}

