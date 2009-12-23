#ifndef DEF_KERNEL_SEGMENT_CLASS_H
#define DEF_KERNEL_SEGMENT_CLASS_H

#include <MemoryManager/Segment.proto.h>
#include <MemoryManager/PageDirectory.class.h>

class KernelSegment : public AllocaterSegment {
	private:
	Vector<PageDirectory*> m_pds;
	page_table_t *tables[256];
	u32int tablesPhysical[256];

	page_t* getPage(u32int addr);
	
	public:
	KernelSegment();

	seg_map_t* map(PageDirectory *pd);
	void unmap(seg_map_t *mapping) {}
	bool handleFault(u32int addr, bool write, seg_map_t *mapping) { return false; }

	bool allocFrame(u32int addr);	//false when page already was mapped to a frame
	void mapFrame(u32int addr, u32int phys);	//For use by drivers
	void freeFrame(u32int addr);
};

#endif
