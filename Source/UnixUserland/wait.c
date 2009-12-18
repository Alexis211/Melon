#include <errno.h>
#undef errno
extern int errno;

int wait(int *status) {
	errno = ECHILD;
	return -1;
}
