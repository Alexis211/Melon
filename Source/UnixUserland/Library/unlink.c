#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int unlink(char *name) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_UNLINK), "b"(name));
	if (ret != 0) errno = ENOENT;
	return ret;
}
