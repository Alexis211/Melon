#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int link(char *old, char *new) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_LINK), "b"(old), "c"(new));
	if (ret != 0) errno = EMLINK;
	return ret;
}
