#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "ptrace.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64 sys_waitpid(void)
{
  int pid;
  uint64 status;
  int options;

  if (argint(0, &pid) < 0) {
    return -1;
  }
  if (argaddr(1, &status) < 0) {
    return -1;
  }
  if (argint(2, &options) < 0) {
    return -1;
  }

  return waitpid(pid, status, options);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
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

/*
 * Function signature:
 * uint64 ptrace(int request, int pid, void *addr, void *data);
 */
uint64 sys_ptrace(void)
{
    int request;
    int pid;
    uint64 addr;
    uint64 data;

    if (argint(0, &request) < 0) {
        return -1;
    }
    if (argint(1, &pid) < 0) {
        return -1;
    }
    if (argaddr(2, &addr) < 0) {
        return -1;
    }
    if (argaddr(3, &data) < 0) {
        return -1;
    }

    return ptrace(request, pid, addr, data);
}
