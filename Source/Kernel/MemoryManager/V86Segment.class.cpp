#include "V86Segment.class.h"
#include <MemoryManager/PageDirectory.class.h>

seg_map_t V86Segment::map(PageDirectory *pd) {
	for (u32int i = 0; i < 0x100000; i += 0x1000) {
		PageDirectory::map(pd->getPage(i, true), i / 0x1000, true, true);
	}
	seg_map_t map = {this, 0, 0x100000};
	return map;
}

void V86Segment::unmap(PageDirectory* pd, seg_map_t *mapping) {
	for (u32int i = 0; i < 0x100000; i += 0x1000) {
		PageDirectory::unmap(pd->getPage(i, false));
	}
}
