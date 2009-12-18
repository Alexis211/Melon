#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int kill(int pid, int sig) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_KILL), "b"(pid), "c"(sig));
	if (ret != 0) errno = EINVAL;
	return ret;
}
