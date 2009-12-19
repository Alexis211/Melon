#include <Unix.iface.h>

void _exit(int errcode) {
	asm volatile("int $63;" : : "a"(UNIX_SC_EXIT), "b"(errcode));
}
