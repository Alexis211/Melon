#include <Unix.iface.h>

int lseek(int file, int ptr, int dir) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_LSEEK), "b"(file), "c"(ptr), "d"(dir));
	return ret;
}
