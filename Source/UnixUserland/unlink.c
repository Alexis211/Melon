#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int unlink(char *name) {
	errno = ENOENT;
	asm volatile("int $63;" : : "a"(UNIX_SC_UNLINK), "b"(name));
	return -1;
}
