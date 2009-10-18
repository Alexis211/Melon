#include "Syscall.wtf.h"

int main();

unsigned int syscall(unsigned int n, unsigned int a, unsigned int b, unsigned int c) {
	unsigned int r;
	asm volatile ("int $64;"
			: "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c));
	return r;
}

extern "C" void start() {
	unsigned int r = main();
	asm volatile("int $66" : : "a"(r));
}

void putch(char c) {
	unsigned int x = c;
	syscall(0xFFFFFF01, x);
}
