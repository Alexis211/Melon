#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int link(char *old, char *new) {
	errno = EMLINK;
	asm volatile("int $63;" : : "a"(UNIX_SC_LINK), "b"(old), "c"(new));
	return -1;
}
