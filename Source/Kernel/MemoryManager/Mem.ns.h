#ifdef DEF_COMMON

#ifndef DEF_MEM_NS_H
#define DEF_MEM_NS_H

namespace Mem {
	extern bool pagingEnabled;
	extern u32int placementAddress;

	void createHeap();
	void *kalloc(u32int sz, bool align = false);
	void kfree(void *ptr);

	u32int kheapSize(), kheapFree();
}

#endif

#endif
