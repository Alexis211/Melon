#include  <sys/types.h>

caddr_t sbrk(int incr) {
	extern char end;
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &end;
	}
	prev_heap_end = heap_end;

	heap_end += incr;
	return (caddr_t) prev_heap_end;

}
