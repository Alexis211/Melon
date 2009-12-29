#ifndef DEF_SYSCALL_WTF_H
#define DEF_SYSCALL_WTF_H

#include <types.h>

void threadFinishedSyscall(u32int);
void breakPoint();

u32int syscall(u32int n, u32int a, u32int b = 0, u32int c = 0, u32int d = 0, u32int e = 0);

#endif
