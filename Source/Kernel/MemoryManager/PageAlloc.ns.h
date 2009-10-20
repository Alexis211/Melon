#ifndef DEF_PAGEALLOC_NS_H
#define DEF_PAGEALLOC_NS_H

#include <common.h>

namespace PageAlloc {
	void init();
	void* alloc(u32int* phys);
	void free(void *ptr);
}

#endif
