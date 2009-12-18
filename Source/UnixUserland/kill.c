#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int kill(int pid, int sig) {
	errno = EINVAL;
	asm volatile("int $63;" : : "a"(UNIX_SC_KILL), "b"(pid), "c"(sig));
	return -1;
}
