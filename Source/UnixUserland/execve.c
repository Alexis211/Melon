#include <Unix.iface.h>

#include <errno.h>
#undef errno
extern int errno;

int execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	asm volatile("int $63;" : : "a"(UNIX_SC_EXECVE), "b"(name), "c"(argv), "d"(env));
	return -1;
}
