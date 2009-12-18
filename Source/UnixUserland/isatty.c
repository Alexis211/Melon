#include <Unix.iface.h>

int isatty(int file) {
	file++;
	return 1;
}
