 #include <Unix.iface.h>

int read(int file, char *ptr, int len) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_READ), "b"(file), "c"(ptr), "d"(len));
	return ret;
}
