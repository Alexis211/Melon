#ifndef DEF_V86SEGMENT_CLASS_H
#define DEF_V86SEGMENT_CLASS_H

#include <MemoryManager/Segment.proto.h>

class V86Segment : public Segment {
	private:
	V86Segment() {}

	public:
	seg_map_t* map(PageDirectory *pd);
	void unmap(seg_map_t *mapping);
	bool handleFault(u32int addr, seg_map_t *mapping) { return false; }

	static V86Segment seg;
};

#endif
