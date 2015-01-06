#include <pthread.h>

typedef union {
    pthread_rwlock_t oldLock; // Only to keep size
    struct {
        pthread_rwlock_t *originalLock;
        struct rw *numaLock;
        // My fancy data, which must be smaller than (56 - sizeof(pthread_rwlock_t*)) bytes,
        // or a pointer to actual fancy data otherwise
        int node;
    };
} my_pthread_rwlock_t;

struct rw *findLock(pthread_rwlock_t *rwlock);
