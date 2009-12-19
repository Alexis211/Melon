#include <Unix.iface.h>

int getpid() {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_GETPID));
	return ret;
}
