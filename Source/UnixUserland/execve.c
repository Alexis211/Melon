#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int execve(char *name, char **argv, char **env) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_EXECVE), "b"(name), "c"(argv), "d"(env));
	if (ret != 0) errno = ENOMEM;
	return ret;
}
