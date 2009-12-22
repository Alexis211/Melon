#ifndef DEF_PHYSMEM_NS_H
#define DEF_PHYSMEM_NS_H

#include <MemoryManager/PageDirectory.class.h>

#include <MemoryManager/KernelSegment.class.h>

extern PageDirectory* kernelPageDirectory;

namespace PhysMem {
	void initPaging(u32int mem_size);
	void removeTemporaryPages();

	u32int getFrame();
	void freeFrame(u32int frame);

	u32int free();
	u32int total();

	extern KernelSegment keSeg;
}

#endif
