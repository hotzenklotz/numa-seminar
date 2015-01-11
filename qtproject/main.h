#pragma once
#define pthread_rwlock_destroy old_pthread_rwlock_destroy
#define pthread_rwlock_rdlock old_pthread_rwlock_rdlock
#define pthread_rwlock_wrlock old_pthread_rwlock_wrlock
#define pthread_rwlock_unlock old_pthread_rwlock_unlock
#define pthread_rwlock_init old_pthread_rwlock_init
#include <pthread.h>
#undef pthread_rwlock_destroy
#undef pthread_rwlock_rdlock
#undef pthread_rwlock_wrlock
#undef pthread_rwlock_unlock
#undef pthread_rwlock_init

typedef struct {
	struct rw *numaLock;
	// My fancy data, which must be smaller than (56 - sizeof(pthread_rwlock_t*)) bytes,
	// or a pointer to actual fancy data otherwise
	int node;
} my_pthread_rwlock_t;
