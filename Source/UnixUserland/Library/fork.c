#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int fork() {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_FORK));
	if (ret != 0) errno = EAGAIN;
	return ret;
}
