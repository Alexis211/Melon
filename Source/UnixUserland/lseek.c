#include <Unix.iface.h>

int lseek(int file, int ptr, int dir) {
	asm volatile("int $63;" : : "a"(UNIX_SC_LSEEK), "b"(file), "c"(ptr), "d"(dir));
	return 0;
}
