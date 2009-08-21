#ifdef DEF_COMMON

#ifndef DEF_MEM_NS_H
#define DEF_MEM_NS_H

namespace Mem {
	extern u32int placementAddress, kheapFree;

	void *kalloc(u32int sz, bool align = false);
	void kfree(void *ptr);

}

#endif

#endif
