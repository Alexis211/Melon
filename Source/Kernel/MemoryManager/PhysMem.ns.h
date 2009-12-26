#ifndef DEF_PHYSMEM_NS_H
#define DEF_PHYSMEM_NS_H

#include <MemoryManager/PageDirectory.class.h>

extern PageDirectory* kernelPageDirectory;

namespace PhysMem {
	void initPaging(u32int mem_size);
	void removeTemporaryPages();

	void allocFrame(page_t *page, bool is_user, bool is_writable);
	void freeFrame(page_t *page);

	u32int free();
	u32int total();
}

#endif
