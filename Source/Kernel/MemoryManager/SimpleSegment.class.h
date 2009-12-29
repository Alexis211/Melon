#ifndef SIMPLESEGMENT_CLASS_H
#define SIMPLESEGMENT_CLASS_H

#include <MemoryManager/Segment.proto.h>

/*
 * This class implements a simple resizeable segment that can be mapped to only one page directory.
 * It will probably not exist when segmentation evolves.
 */

class SimpleSegment : public AllocaterSegment {
	private:
	bool m_user, m_rw;
	seg_map_t *m_mapping;
	u32int m_start, m_len;

	public:
	SimpleSegment(bool user, bool rw, u32int start, u32int len) : 
		m_user(user), m_rw(rw), m_mapping(0), m_start(start), m_len(len) {}
	~SimpleSegment() {}

	seg_map_t *map(PageDirectory* pd);
	void unmap(seg_map_t *mapping);
	bool handleFault(u32int addr, bool write, seg_map_t *mapping);

	bool allocFrame(u32int addr);
	void freeFrame(u32int addr);
	virtual void switchToPd();

	Segment* clone() { return 0; }	//Segment cannot be cloned
};

#endif
