#include "V86Segment.class.h"
#include <MemoryManager/PageDirectory.class.h>
#include <VTManager/SimpleVT.class.h>

V86Segment V86Segment::seg;

seg_map_t *V86Segment::map(PageDirectory *pd) {
	for (u32int i = 0; i < 0x100000; i += 0x1000) {
		page_t *p = pd->getPage(i, true);
		PageDirectory::map(p, i / 0x1000, true, true);
	}
	seg_map_t *map = new seg_map_t();
	map->seg = this;
	map->start = 0;
	map->len = 0x100000;
	map->pd = pd;
	return map;
}

void V86Segment::unmap(seg_map_t *mapping) {
	for (u32int i = 0; i < 0x100000; i += 0x1000) {
		PageDirectory::unmap(mapping->pd->getPage(i, false));
	}
}
