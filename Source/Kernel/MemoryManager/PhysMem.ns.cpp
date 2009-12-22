#include "PhysMem.ns.h"
#include <Bitset.class.h>
#include <VTManager/VirtualTerminal.proto.h>

PageDirectory* kernelPageDirectory;

namespace PhysMem {

u32int nframes;
Bitset *frames;

KernelSegment keSeg;

void initPaging(u32int mem_size) {
	nframes = mem_size / 0x1000;
	
	frames = new Bitset(nframes);

	for (u32int i = 0; i < Mem::placementAddress / 0x1000; i++) frames->setBit(i);

	kernelPageDirectory = new (Mem::alloc(sizeof(PageDirectory), true)) PageDirectory();
	kernelPageDirectory->map(&keSeg);

	u32int i = 0xC0000000;
	while (i < Mem::placementAddress) {
		keSeg.identityMap(i);
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

u32int getFrame() {
	u32int idx = frames->firstFreeBit();
	if (idx == (u32int) - 1) PANIC("No more free frames !");
	frames->setBit(idx);
	return idx;
}

void freeFrame(u32int frame) {
	frames->clearBit(frame);
}

u32int free() {
	return nframes - frames->usedBits();
}

u32int total() {
	return nframes;
}

}
