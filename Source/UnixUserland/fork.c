#include <errno.h>
#undef errno
extern int errno;

int fork() {
	errno = EAGAIN;
	return -1;
}
