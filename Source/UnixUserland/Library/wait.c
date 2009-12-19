#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int wait(int *status) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_WAIT), "b"(status));
	if (ret != 0) errno = ECHILD;
	return ret;
}
