#include <pthread.h>
#include <stdio.h>
/* Acquire write lock for RWLOCK.  */
int pthread_rwlock_wrlock (pthread_rwlock_t *rwlock)
{
  int result = 0;
  printf("CUSTOM WRITE LOCK ACQUIRED\n");

  return result;
}

int pthread_rwlock_unlock (pthread_rwlock_t *rwlock)
{
    int result = 0;
    printf("CUSTOM RW UNLOCK\n");
  return result;
}

int pthread_rwlock_rdlock (pthread_rwlock_t *rwlock)
{
  int result = 0;
  printf("CUSTOM READ LOCK ACQUIRED\n");

  return result;
}