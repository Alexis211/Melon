#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int wait(int *status) {
	errno = ECHILD;
	asm volatile("int $63;" : : "a"(UNIX_SC_WAITPID), "b"(status));
	return -1;
}
