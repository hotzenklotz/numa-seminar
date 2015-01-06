#include <stdio.h>
#define __USE_UNIX98
#include <pthread.h>
#include <main.h>
#include <rw_bo_msc.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static my_pthread_rwlock_t thread_array[100000];
static int index = 0;


/* Acquire write lock for RWLOCK.  */
int pthread_rwlock_wrlock (pthread_rwlock_t *rwlock)
{

  printf("CUSTOM WRITE LOCK ACQUIRED\n");
  my_pthread_rwlock_t *myRWLock = findLock(rwlock);
  rw_acquire(myRWLock->numaLock, W, myRWLock->node);
  return 0;
}

int pthread_rwlock_unlock (pthread_rwlock_t *rwlock)
{

    printf("CUSTOM RW UNLOCK\n");
    my_pthread_rwlock_t *myRWLock = findLock(rwlock);
    rw_release(myRWLock->numaLock, R, myRWLock->node);
    return 0;

}

int pthread_rwlock_rdlock (pthread_rwlock_t *rwlock)
{
  printf("CUSTOM READ LOCK ACQUIRED\n");
  my_pthread_rwlock_t *myRWLock = findLock(rwlock);
  rw_acquire(myRWLock->numaLock, R, myRWLock->node);
  return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    my_pthread_rwlock_t *myRWLock = findLock(rwlock);
    destroy_rw(myRWLock->numaLock);

}

/* every original pthread is the 'key' for our numa-aware lock. Do a look-up. */
struct rw *findLock(pthread_rwlock_t *rwlock)
{
    struct rw *result;
    for (int i; i <= index; i++)
    {
        if (rwlock == thread_array[i].originalLock)
        {
            result = thread_array[i].numaLock;
        }
    }
    return result;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr){

   my_pthread_rwlock_t *myRWLock = malloc(sizeof(my_pthread_rwlock_t));
   myRWLock->originalLock = rwlock;
   myRWLock->numaLock = init_rw(READER); //malloc(sizeof(rw));
   myRWLock->node = 0;

   thread_array[index] = *myRWLock;

   //increment the alookup-array index in a thread-safe manner
   int rc = pthread_mutex_lock(&mutex);
   index++;
   rc = pthread_mutex_unlock(&mutex);

  return 0;
}

