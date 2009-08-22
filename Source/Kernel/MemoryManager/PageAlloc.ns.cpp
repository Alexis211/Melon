#include "PageAlloc.ns.h"
#include <MemoryManager/PhysMem.ns.h>

#define CACHED_PAGES 3

// This mechanism is supposed to make it impossible to run out of free pages to use as a page table

namespace PageAlloc {

void* freePage[CACHED_PAGES];
int freec = 0;
bool usable = false, locked = false;

void init() {
	freec = CACHED_PAGES;
	for (u32int i = 0; i < CACHED_PAGES; i++) {
		freePage[i] = Mem::kalloc(0x1000, true);
	}
	usable = true;
}

void* alloc(u32int* phys) {
	if (!usable) init();

	while (freec < CACHED_PAGES && (!locked)) {
		locked = true;
		void* next = 0;
		if (!Mem::pagingEnabled) {
			next = Mem::kalloc(0x1000, true);
		} else {
			u32int i = 0xFFFFF000;
			page_t *p;
			while (1) {
				p = kernelPageDirectory->getPage(i, true);
				if (p->frame == 0) break;
				i -= 0x1000;
			}
			PhysMem::allocFrame(p, true, false);
			next = (void*)i;
		}
		freePage[freec] = next;
		freec++;
		locked = false;
	}

	freec--;
	void* ret = freePage[freec];
	if (!Mem::pagingEnabled) {
		*phys = (u32int)ret - 0xC0000000;
	} else {
		page_t* p = kernelPageDirectory->getPage((u32int)ret, false);
		if (p == 0) {	//THIS SHOULD NEVER HAPPEN
			PANIC("Cached free page does not exist.");
		} else if (p->frame == 0) {
			PANIC("Cached free page is not mapped to a frame.");
		} else {
			*phys = (p->frame * 0x1000);
		}
	}
	return ret;
}

void free(void *ptr) {
	kernelPageDirectory->freeFrame((u32int)ptr);
	return;
}

}
