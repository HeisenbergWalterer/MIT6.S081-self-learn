// 专门用于实现进程和系统相关的系统调用
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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
  系统调用的参数不是通过函数参数传递的，而是通过CPU寄存器传递：
  
  用户程序调用：
  trace(32);
  实际发生的过程：
  1. 用户程序将参数32放入寄存器a0
  2. 将系统调用号SYS_trace放入寄存器a7  
  3. 执行ecall指令进入内核
  4. 内核从寄存器中读取参数，而不是从函数参数
*/

// trace的作用相当于给一个进程赋一个mask值来决定在syscall执行时是否跟踪它（输出跟踪信息）
uint64
sys_trace(void) { // 所有的系统调用都是void参数
  int mask;
  if(argint(0, &mask) < 0) {
    return -1;
  }

  myproc()->trace_mask = mask; // myproc:返回当前进程控制块指针

  return 0;
}

uint64
sys_sysinfo(void) {
  uint64 addr;           // 存放用户空间的目标地址
  struct sysinfo info;  // 需传递的数据
  struct proc *p = myproc();

  if(argaddr(0, &addr) < 0) {
    return -1;
  }

  info.freemem = getFreeMem();
  info.nproc = getNproc();

  // 将数据复制到用户空间
  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0) {
    return -1;
  }
  
  return 0;
}