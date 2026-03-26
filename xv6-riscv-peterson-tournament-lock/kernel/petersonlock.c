#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h" 
#include "proc.h"
#include "petersonlock.h"

#define MAX_LOCKS 15
struct petersonlock locks[MAX_LOCKS];

// initialaizing all of the petersonlocks in the kernel;
void 
petersoninit(void)
{
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        locks[i].used = 0;
        locks[i].flag[0] = 0;
        locks[i].flag[1] = 0;
        locks[i].turn = 0;
    }
}

// searching for the first "unused" lock , returning its index;
int 
peterson_create(void)
{
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        if (locks[i].used == 0)
        {
            locks[i].used = 1;
            locks[i].flag[0] = 0;
            locks[i].flag[1] = 0;
            locks[i].turn = 0;
            return i; // return lock ID as unique identifier
        }
    }
    return -1; // no available locks
}

// trying to acquire the lock 
int 
peterson_acquire(int lock_id, int role)
{
    if (lock_id < 0 || lock_id >= MAX_LOCKS || locks[lock_id].used == 0 || role < 0 || role > 1)
        return -1;

    // Peterson’s entry protocol
    locks[lock_id].flag[role] = 1;
    __sync_synchronize();
    locks[lock_id].turn = 1 - role;
    //__sync_synchronize();
    while (locks[lock_id].flag[1 - role] && locks[lock_id].turn == 1 - role)
    {
        yield(); // Give up CPU while waiting
    }

    return 0; // Lock acquired
}

// releasing the lock , bu giving the turn to the second process;
int 
peterson_release(int lock_id, int role)
{
    if (lock_id < 0 || lock_id >= MAX_LOCKS || locks[lock_id].used == 0 || role < 0 || role > 1)
        return -1;
    
    __sync_synchronize();
    
    locks[lock_id].flag[role] = 0;
    return 0;
}

// destroying the peterson's lock with the given lock id;
int
peterson_destroy(int lock_id)
{
    if (lock_id < 0 || lock_id >= MAX_LOCKS || locks[lock_id].used == 0)
        return -1;

    __sync_synchronize();
    
    locks[lock_id].used = 0;    
    
    return 0;
}