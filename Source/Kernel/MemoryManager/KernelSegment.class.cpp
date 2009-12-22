#include "KernelSegment.class.h"
#include <MemoryManager/PageAlloc.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageDirectory.class.h>
#include <VTManager/SimpleVT.class.h>

KernelSegment::KernelSegment() {
	for (u32int i = 0; i < 256; i++) {
		tables[i] = 0;
		tablesPhysical[i] = 0;
	}
}

seg_map_t *KernelSegment::map(PageDirectory* pd) {
	for (u32int i = 0; i < 256; i++) {
		pd->tables[i + 768] = tables[i];
		pd->tablesPhysical[i + 768] = tablesPhysical[i];
	}
	m_pds.push(pd);
	seg_map_t *ret = new seg_map_t;
	ret->seg = this;
	ret->pd = pd;
	ret->start = 0xC0000000;
	ret->len = 0x40000000;
	return ret;
}

page_t* KernelSegment::getPage(u32int addr) {
	if (addr < 0xC0000000) return 0;
	addr /= 0x1000;
	u32int tableIdx = addr / 1024 - 768;
	if (tables[tableIdx] == 0) {
		tables[tableIdx] = (page_table_t*)PageAlloc::alloc(tablesPhysical + tableIdx);
		tablesPhysical[tableIdx] |= 0x07;
		CMem::memset((u8int*)tables[tableIdx], 0, 0x1000);
		for (u32int i = 0; i < m_pds.size(); i++) {
			m_pds[i]->tables[tableIdx + 768] = tables[tableIdx];
			m_pds[i]->tablesPhysical[tableIdx + 768] = tablesPhysical[tableIdx];
		}
	}
	return &tables[tableIdx]->pages[addr % 1024];
}

bool KernelSegment::allocFrame(u32int addr) {
	page_t *p = getPage(addr);
	if (p == 0) return false;
	if (p->frame != 0) return false;
	PageDirectory::map(p, PhysMem::getFrame(), false, false);
	return true;
}

void KernelSegment::mapFrame(u32int addr, u32int phys) {
	page_t *p = getPage(addr);
	if (p == 0) return;
	if (p->frame != 0) return;
	PageDirectory::map(p, phys / 0X1000, false, false);
}

void KernelSegment::freeFrame(u32int addr) {
	if (addr < 0xC0000000) return;
	addr /= 0x1000;
	u32int tableIdx = addr / 1024 - 768;
	if (tables[tableIdx] == 0) return;
	PageDirectory::unmap(&tables[tableIdx]->pages[addr % 1024], true);
}
