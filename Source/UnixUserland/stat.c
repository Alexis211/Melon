#include <Unix.iface.h>

#include <sys/stat.h>

int stat(const char *file, struct stat *st) {
	asm volatile("int $63;" : "=a"(st->st_mode) : "a"(UNIX_SC_STAT), "b"(file));
	return 0;
}
