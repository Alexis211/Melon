#include <Unix.iface.h>

#include <sys/times.h>

clock_t times(struct tms *buff) {
	int ret;
	asm volatile("int $63;" : "=a"(ret) : "a"(UNIX_SC_TIMES));
	return ret;
}
