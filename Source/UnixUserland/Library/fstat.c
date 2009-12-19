#include <Unix.iface.h>

#include <sys/stat.h>

int fstat(int file, struct stat *st) {
	asm volatile("int $63;" : "=a"(st->st_mode) : "a"(UNIX_SC_FSTAT), "b"(file));
	return 0;
}
