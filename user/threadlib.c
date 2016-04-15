#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "threadlib.h"

void thread_create(void (*start_routine)(void*), void *arg)
{


}

void thread_join ()
{
	
}

void lock_init(lock_t * padlock)
{
	padlock->locked = 0;
	padlock->cpu = 0;	
}

void lock_acquire(lock_t * padlock)
{
    pushcli(); // disable interrupts to avoid deadlock.
    if(holding(lk))
      panic("acquire");

    // The xchg is atomic.
    // It also serializes, so that reads after acquire are not
    // reordered before it.
    while(xchg(&padlock->locked, 1) != 0)
      ;

    // Record info about lock acquisition for debugging.
    padlock->cpu = cpu;
    getcallerpcs(&padlock, padlock->pcs);
}

void lock_release(lock_t * padlock)
{
    if(!holding(padlock))
      panic("release");

    padlock->pcs[0] = 0;
    padlock->cpu = 0;

    // The xchg serializes, so that reads before release are
    // not reordered after it.  The 1996 PentiumPro manual (Volume 3,
    // 7.2) says reads can be carried out speculatively and in
    // any order, which implies we need to serialize here.
    // But the 2007 Intel 64 Architecture Memory Ordering White
    // Paper says that Intel 64 and IA-32 will not move a load
    // after a store. So lock->locked = 0 would work here.
    // The xchg being asm volatile ensures gcc emits it after
    // the above assignments (and after the critical section).
    xchg(&lk->locked, 0);

    popcli();
}

// Check whether this cpu is holding the lock.
int
holding(lock_t *lock)
{
  return lock->locked && lock->cpu == cpu;
}
