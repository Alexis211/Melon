#include "Syscall.wtf.h"

u32int syscall(u32int n, u32int a, u32int b, u32int c, u32int d, u32int e) {
	u32int r;
	asm volatile ("int $64;"
			: "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c), "D"(d), "S"(e));
	return r;
}

void threadFinishedSyscall(u32int v) {
	asm volatile("int $66" : : "a"(v));
}

void putch(char c) {
	u32int x = c;
	syscall(0xFFFFFF01, x);
}

void sleep(u32int t) {
	syscall(0xFFFFFF02, t);
}

void write_hex(u32int n) {
	syscall(0XFFFFFF03, n);
}
