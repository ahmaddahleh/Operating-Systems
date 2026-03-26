#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  char msg[32];
  argint(0, &n);
  if (argstr(1, msg, sizeof(msg)) < 0)
     return -1;
  exit(n , msg);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  uint64 msg_ptr;
  argaddr(0, &p);         // for the status
  argaddr(1, &msg_ptr);   // for the message buffer (user space)

  return wait(p, (char*)msg_ptr);  // cast only for type match
}


uint64
sys_sbrk(void)
{
  uint64 addr; 
  int n;
  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// returns the process size in bytes using the field in PCB
// task 2 
uint64
sys_memsize(void)
{
  return myproc()->sz;
}

//task 4 : 
uint64
sys_forkn(void)
{
  int n; // number of processes
  uint64 pids_ptr; // pointer for processes id

  argint(0, &n);
  argaddr(1, &pids_ptr); 
  return forkn(n , (int*)pids_ptr);
}

uint64
sys_waitall(void)
{
  uint64 n; // number of processes that finished working
  uint64 pstat_ptr; // pointer for processes statuses

  argaddr(0, &n);
  argaddr(1, &pstat_ptr); 
  return waitall((int*) n , (int*) pstat_ptr);
}