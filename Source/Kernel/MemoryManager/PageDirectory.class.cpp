#include "PageDirectory.class.h"
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <TaskManager/Task.ns.h>

extern "C" void copy_page_physical(u32int src, u32int dest);

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

/* 			FOR PAGE DIRECTORES				*/
PageDirectory::PageDirectory() {
	tablesPhysical = (u32int*)PageAlloc::alloc(&physicalAddr);
	for (u32int i = 0; i < 1024; i++) {
		tables[i] = 0;
		tablesPhysical[i] = 0;
	}
}

PageDirectory::PageDirectory(PageDirectory* other) {
	tablesPhysical = (u32int*)PageAlloc::alloc(&physicalAddr);
	for (u32int i = 0; i < 768; i++) {
		if (other->tablesPhysical[i] != 0) {
			u32int tmp;
			tables[i] = (page_table_t*)PageAlloc::alloc(&tmp);
			tablesPhysical[i] = tmp | 0x07;
			for (u32int j = 0; j < 1024; j++) {
				if (!(other->tables[i]->pages[j].frame))
					continue;
				if (i == 0 and j < 256) continue;	//Frame is below 1M, probably used by some V86 stuff. Ignore it.
				map(&tables[i]->pages[j], PhysMem::getFrame(), true, true);
				tables[i]->pages[j].present = other->tables[i]->pages[j].present;
				tables[i]->pages[j].rw = other->tables[i]->pages[j].rw;
				tables[i]->pages[j].user = other->tables[i]->pages[j].user;
				tables[i]->pages[j].accessed = other->tables[i]->pages[j].accessed;
				tables[i]->pages[j].dirty = other->tables[i]->pages[j].dirty;
				copy_page_physical(other->tables[i]->pages[j].frame * 0x1000, tables[i]->pages[j].frame * 0x1000);
			}
		} else {
			tables[i] = 0;
			tablesPhysical[i] = 0;
		}
	}
	for (u32int i = 768; i < 1024; i++)	{	//Link kernel page tables
		tablesPhysical[i] = other->tablesPhysical[i];
		tables[i] = other->tables[i];
	}
}

PageDirectory::~PageDirectory() {
	for (u32int i = 0; i < mappedSegs.size(); i++) {
		mappedSegs[i].seg->unmap(this, &mappedSegs[i]);
	}
	for (int i = 0; i < 768; i++) {		//Only free addresses below 0xC0000000, upper is kernel space
		if (tables[i] != 0) {
			PageAlloc::free((void*)tables[i]);
		}
	}
	PageAlloc::free((void*)tablesPhysical);
}

void PageDirectory::map(Segment* seg) {
	mappedSegs.push(seg->map(this));
}

void PageDirectory::unmap(Segment* seg) {
	for (u32int i = 0; i < mappedSegs.size(); i++) {
		if (mappedSegs[i].seg == seg) {
			seg->unmap(this, &mappedSegs[i]);
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
		if (address >= 0xC0000000) return 0;
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
	asm volatile("mov %0, %%cr3" :: "r"(physicalAddr));
	u32int cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
