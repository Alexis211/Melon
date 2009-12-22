#ifndef DEF_SEGMENT_PROTO_H
#define DEF_SEGMENT_PROTO_H

#include <common.h>

class PageDirectory;
class Segment;

struct seg_map_t {
	Segment* seg;
	u32int start;
	u32int len;
};

class Segment {
	public:
	virtual seg_map_t map(PageDirectory *pd) = 0;
	virtual void unmap(PageDirectory *pd, seg_map_t *mapping) = 0;
	virtual bool handleFault(u32int addr, seg_map_t *mapping) = 0;	//true = ok, false = app segfaulted

	virtual Segment* clone() { return this; }
};

#endif
