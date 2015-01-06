#include <stdbool.h>

//##### General ##################

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


//##### READ_LOCK ###################
#define PATIENCE 1000

typedef enum _mode {R, W} mode;
typedef enum _pref {NEUTRAL, READER, WRITER, READER_OPT} preference;

struct readindr {
  unsigned arrive;
  unsigned depart;
  char pad[56];
};

struct rw {
  struct readindr *indicators;
  struct c_bo_mcs *writers;
  preference pref;
  int rbarrier;
  int wbarrier;
  bool wactive;
#ifdef __x86_64__
  char pad[28];
#else
  char pad[40];
#endif
};

// ##### Backoff Lock ################
#define BACKOFF_FREE -1
#define BACKOFF_BASE 100
#define BACKOFF_CAP 1000
#define BACKOFF_FACTOR 2
#define REMOTE_BACKOFF_BASE 500
#define REMOTE_BACKOFF_CAP 5000

struct bo {
  volatile int lock;
  int state;
  int visitors;
  bool successor_exists;
  int gstate;
  char pad[47];
};

//############### MCS Lock ##############

struct mcs {
  int state;
  unsigned long visitors;
  struct mcs *next;
  struct mcs *gstate;
#ifdef __x86_64__
  char pad[40];
#else
  char pad[48];
#endif
};

// ######### Cohort Lock (BO-MCS) #####

struct c_bo_mcs {
  struct bo *glock;
  struct mcs **llocks;
#ifdef __x86_64__
  char pad[48];
#else
  char pad[56];
#endif
};

// ################## methods ###########

struct rw *init_rw(preference p);
void destroy_rw(struct rw *l);
void rw_release(struct rw *l, mode m, int node);
void rw_acquire(struct rw *l, mode m, int *node);
