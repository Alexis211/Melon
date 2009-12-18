#include <Unix.iface.h>

int write(int file, char *ptr, int len) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_WRITE), "b"(file), "c"(ptr), "d"(len));
	return ret;
}
