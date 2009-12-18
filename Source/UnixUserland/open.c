#include <Unix.iface.h>

int open(const char* name, int flags, int mode) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_OPEN), "b"(name), "c"(flags), "d"(mode));
	return ret;
}
