#ifndef DEF_SYSCALL_WTF_H
#define DEF_SYSCALL_WTF_H

#include <types.h>

void putch(char);
unsigned int syscall(unsigned int n, unsigned int a, unsigned int b = 0, unsigned int c = 0);

#endif
