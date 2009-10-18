#include "PhysMem.ns.h"
#include <Library/Bitset.class.h>
#include <VTManager/VirtualTerminal.proto.h>

PageDirectory* kernelPageDirectory;

namespace PhysMem {

u32int nframes;
Bitset *frames;

void initPaging(u32int mem_size) {
	nframes = mem_size / 0x1000;
	
	frames = new Bitset(nframes);

	kernelPageDirectory = new (Mem::kalloc(sizeof(PageDirectory), true)) PageDirectory();

	u32int i = 0xC0000000;
	while (i < Mem::placementAddress) {
		page_t *p2 = kernelPageDirectory->getPage(i, true);
		allocFrame(p2, false, false);
		i += 0x1000;
	}
	//Also map thoses pages at begning of virtual memory
	for (u32int i = 0; i < (Mem::placementAddress - 0xC0000000) / 0x100000; i++) {
		kernelPageDirectory->tablesPhysical[i] = kernelPageDirectory->tablesPhysical[768 + i];
		kernelPageDirectory->tables[i] = kernelPageDirectory->tables[768 + i];
	}
	DEBUG_HEX((u32int)kernelPageDirectory->physicalAddr); DEBUG(" is page dir phys addr.");

	kernelPageDirectory->switchTo();
	DEBUG("Paging enabled !");

	Mem::pagingEnabled = true;
}

void removeTemporaryPages() {
	for (u32int i = 0; i < (Mem::placementAddress - 0xC0000000) / 0x100000; i++) {
		kernelPageDirectory->tablesPhysical[i] = 0;
		kernelPageDirectory->tables[i] = 0;
	}
}

void allocFrame(page_t *page, bool is_user, bool is_writable) {
	if (page->frame != 0) {
		return;
	} else {
		u32int idx = frames->firstFreeBit();
		if (idx == (u32int) - 1) PANIC("No more free frames !");
		frames->setBit(idx);
		page->present = 1;
		page->user = (is_user ? 1 : 0);
		page->rw = (is_writable ? 1 : 0);
		page->frame = idx;
	}
}

void freeFrame(page_t *page) {
	if (page->frame == 0) {
		return;
	} else {
		if (page->frame >= 0x100) {	//First 1M are reserved (system)
			frames->clearBit(page->frame);
		}
		page->frame = 0;
	}
}

u32int free() {
	return nframes - frames->usedBits();
}

u32int total() {
	return nframes;
}

}
