#include <Unix.iface.h>

#include <sys/stat.h>

int fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	asm volatile("int $63;" : : "a"(UNIX_SC_FSTAT), "b"(file));
	return 0;
}
