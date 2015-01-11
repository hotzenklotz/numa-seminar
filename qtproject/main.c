#include <stdio.h>
#include <untitled.h>
#include <rw_bo_msc.h>

/* Acquire write lock for RWLOCK.  */
int pthread_rwlock_wrlock (my_pthread_rwlock_t *rwlock)
{
  printf("CUSTOM WRITE LOCK ACQUIRED\n");
  rwlock->lockType = W;
  rw_acquire(rwlock->numaLock, W, &rwlock->node);
  return 0;
}

int pthread_rwlock_unlock (my_pthread_rwlock_t *rwlock)
{
	printf("CUSTOM RW UNLOCK\n");
	rw_release(rwlock->numaLock, rwlock->lockType, rwlock->node);
	return 0;
}

int pthread_rwlock_rdlock (my_pthread_rwlock_t *rwlock)
{
  printf("CUSTOM READ LOCK ACQUIRED\n");
  rwlock->lockType = R;
  rw_acquire(rwlock->numaLock, R, &rwlock->node);
  return 0;
}

int pthread_rwlock_destroy(my_pthread_rwlock_t *rwlock)
{
	destroy_rw(rwlock->numaLock);
	return 0;
}



typedef int pthread_rwlockattr_t; // Bei Gelegenheit an das Original aus pthread.h anpassen
int pthread_rwlock_init(my_pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{

   rwlock->numaLock = init_rw(READER);
   rwlock->node = 0;

  return 0;
}

