#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int fork() {
	errno = EAGAIN;
	asm volatile("int $63;" : : "a"(UNIX_SC_FORK));
	return -1;
}
