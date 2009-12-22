#ifndef DEF_V86SEGMENT_CLASS_H
#define DEF_V86SEGMENT_CLASS_H

#include <MemoryManager/Segment.proto.h>

class V86Segment : public Segment {
	seg_map_t map(PageDirectory *pd);
	void unmap(PageDirectory* pd, seg_map_t *mapping);
	bool handleFault(u32int addr, seg_map_t *mapping) { return false; }
};

#endif
