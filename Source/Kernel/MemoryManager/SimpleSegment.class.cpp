#include "SimpleSegment.class.h"
#include <MemoryManager/PageDirectory.class.h>
#include <MemoryManager/PhysMem.ns.h>

seg_map_t *SimpleSegment::map(PageDirectory* pd) {
	if (m_mapping != 0) return 0;
	seg_map_t *map = new seg_map_t;
	map->seg = this;
	map->pd = pd;
	map->start = m_start;
	map->len = m_len;
	m_mapping = map;
	return map;
}

void SimpleSegment::unmap(seg_map_t *mapping) {
	if (m_mapping != mapping) return;
	for (u32int i = mapping->start; i < mapping->start + mapping->len; i += 0x1000) {
		page_t* p = mapping->pd->getPage(i, false);
		if (p != 0 && p->frame != 0) {
			PhysMem::freeFrame(p->frame);
			PageDirectory::unmap(p);
		}
	}
}

bool SimpleSegment::handleFault(u32int addr, seg_map_t *mapping) {
	if (m_mapping != mapping) return false;
	if (addr >= mapping->start and addr < mapping->start + mapping->len) {
		PageDirectory::map(mapping->pd->getPage(addr, true), PhysMem::getFrame(), m_user, m_rw);
		return true;
	}
	return false;
}

bool SimpleSegment::allocFrame(u32int addr) {
	if (addr & 0xFFF) addr &= 0xFFFFF000;
	if (addr < m_mapping->start) {
		m_mapping->len += (m_mapping->start - addr);
		m_mapping->start = addr;
	} else if (addr >= m_mapping->start + m_mapping->len) {
		m_mapping->len = addr - m_mapping->start + 0x1000;
		return true;
	}
	return false;
}

void SimpleSegment::freeFrame(u32int addr) {	//Don't update info in m_mapping, we are a *simple* segment
	if (addr & 0xFFF) addr &= 0xFFFFF000;
	if (addr < m_mapping->start or addr >= m_mapping->start + m_mapping->len) return;
	page_t* p = m_mapping->pd->getPage(addr, false);
	if (p != 0 && p->frame != 0) {
		PhysMem::freeFrame(p->frame);
		PageDirectory::unmap(p);
	}
}

void SimpleSegment::switchToPd() {
	if (m_mapping != 0) m_mapping->pd->switchTo(); 
}
