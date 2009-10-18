#ifndef DEF_SYSCALL_WTF_H
#define DEF_SYSCALL_WTF_H

#include <types.h>

//Three basic syscalls, just for testing
void putch(char);
void sleep(u32int);
void write_hex(u32int);

u32int syscall(u32int n, u32int a, u32int b = 0, u32int c = 0, u32int d = 0, u32int e = 0);

#endif
