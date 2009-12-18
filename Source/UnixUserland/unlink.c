#include <errno.h>
#undef errno
extern int errno;

int unlink(char *name) {
	errno = ENOENT;
	return -1;
}
