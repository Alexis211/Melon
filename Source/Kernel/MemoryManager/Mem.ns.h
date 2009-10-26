#ifdef DEF_COMMON

#ifndef DEF_MEM_NS_H
#define DEF_MEM_NS_H

namespace Mem {
	extern bool pagingEnabled;
	extern size_t placementAddress;

	void createHeap();
	void *alloc(size_t sz, bool align = false);
	void free(void *ptr);

	void* mkXchgSpace(size_t sz);	//This creates a space between userland and kernel land where data can be exchanged

	u32int kheapSize(), kheapFree();
}

#endif

#endif
