#ifdef DEF_COMMON

#ifndef DEF_MEM_NS_H
#define DEF_MEM_NS_H

namespace Mem {
	extern bool pagingEnabled;
	extern u32int placementAddress;

	void createHeap();
	void *alloc(u32int sz, bool align = false);
	void free(void *ptr);

	void* mkXchgSpace(u32int sz);	//This creates a space between userland and kernel land where data can be exchanged

	u32int kheapSize(), kheapFree();
}

#endif

#endif
