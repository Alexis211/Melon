#include "PageDirectory.class.h"
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <TaskManager/Task.ns.h>
#include <VTManager/SimpleVT.class.h>

PageDirectory* PageDirectory::currPD;

/* 			FOR MAPPING FRAMES TO PAGES     */
void PageDirectory::map(page_t* p, u32int frame, bool is_user, bool is_writable) {
	if (p == 0) return;
	p->present = 1;
	p->user = (is_user ? 1 : 0);
	p->rw = (is_writable ? 1 : 0);
	p->frame = (frame);
}

void PageDirectory::unmap(page_t *p, bool freePhys) {
	if (p == 0) return;
	if (freePhys) PhysMem::freeFrame(p->frame);
	p->present = 0;
	p->frame = 0;
}

/*			FOR HANDLING PAGE FAULTS		*/
bool PageDirectory::handlePageFault(u32int addr, bool write) {
	if (currPD == 0) return false;
	for (u32int i = 0; i < currPD->mappedSegs.size(); i++) {
		seg_map_t *m = currPD->mappedSegs[i];
		if (addr >= m->start && addr < m->start + m->len) {
			if (m->seg->handleFault(addr, write, m)) return true;
		}
	}
	return false;
}

/* 			FOR PAGE DIRECTORES				*/
PageDirectory::PageDirectory() {
	tablesPhysical = (u32int*)PageAlloc::alloc(&physicalAddr);
	for (u32int i = 0; i < 1024; i++) {
		tables[i] = 0;
		tablesPhysical[i] = 0;
	}
}

PageDirectory::~PageDirectory() {
	for (u32int i = 0; i < mappedSegs.size(); i++) {
		mappedSegs[i]->seg->unmap(mappedSegs[i]);
	}
	for (int i = 0; i < 768; i++) {		//Only free addresses below 0xC0000000, upper is kernel space
		if (tables[i] != 0) {
			PageAlloc::free((void*)tables[i]);
		}
	}
	PageAlloc::free((void*)tablesPhysical);
}

void PageDirectory::map(Segment* seg) {
	for (u32int i = 0; i < mappedSegs.size(); i++) {
		if (mappedSegs[i]->seg == seg) return;
	}
	seg_map_t *map = seg->map(this);
	if (map == 0) return;
	mappedSegs.push(map);
}

void PageDirectory::unmap(Segment* seg) {
	for (u32int i = 0; i < mappedSegs.size(); i++) {
		if (mappedSegs[i]->seg == seg) {
			seg->unmap(mappedSegs[i]);
			delete mappedSegs[i];
			mappedSegs[i] = mappedSegs.back();
			return;
		}
	}
}

page_t *PageDirectory::getPage(u32int address, bool make) {
	address /= 0x1000;
	u32int tableIdx = address / 1024;
	if (tables[tableIdx] != 0) {
		return &(tables[tableIdx]->pages[address % 1024]);
	} else if (make) {
		if (address >= 0xC0000) return 0;	//Creating tables up there is the responsibility of KernelSegment.class
		u32int tmp;
		tables[tableIdx] = (page_table_t*)PageAlloc::alloc(&tmp);
		CMem::memset((u8int*)tables[tableIdx], 0, 0x1000);
		tablesPhysical[tableIdx] = tmp | 0x07;
		return &(tables[tableIdx]->pages[address % 1024]);
	} else {
		return 0;
	}
}

void PageDirectory::switchTo() {
	currPD = this;
	asm volatile("mov %0, %%cr3" :: "r"(physicalAddr));
	u32int cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
