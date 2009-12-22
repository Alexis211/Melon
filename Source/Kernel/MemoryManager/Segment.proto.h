#ifndef DEF_SEGMENT_PROTO_H
#define DEF_SEGMENT_PROTO_H

#include <common.h>

class PageDirectory;
class Segment;

struct seg_map_t {
	Segment* seg;
	PageDirectory* pd;
	u32int start;
	u32int len;
};

class Segment {
	public:
	virtual seg_map_t* map(PageDirectory *pd) = 0;
	virtual void unmap(seg_map_t *mapping) = 0;
	virtual bool handleFault(u32int addr, seg_map_t *mapping) = 0;	//true = ok, false = app segfaulted

	virtual Segment* clone() { return this; }
};

class AllocaterSegment : public Segment {
	public:
	virtual bool allocFrame(u32int addr) = 0;	//false when page already was mapped to a frame
	virtual void freeFrame(u32int addr) = 0;
	virtual void switchToPd() {}
};

#endif
