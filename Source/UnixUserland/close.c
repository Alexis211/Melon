#include <Unix.iface.h>

int close(int file) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_CLOSE), "b"(file));
	return ret;
}
