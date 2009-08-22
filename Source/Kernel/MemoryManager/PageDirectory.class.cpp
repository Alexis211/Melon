#include "PageDirectory.class.h"
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>

PageDirectory::PageDirectory() {
	tablesPhysical = (u32int*)PageAlloc::alloc(&physicalAddr);
	for (u32int i = 0; i < 1024; i++) {
		tables[i] = 0;
		tablesPhysical[i] = 0;
	}
}

PageDirectory::~PageDirectory() {
	PageAlloc::free((void*)tablesPhysical);
	for (int i = 0; i < 1024; i++) {
		if (tables[i] != 0) PageAlloc::free((void*)tables[i]);
	}
}

page_t *PageDirectory::getPage(u32int address, bool make) {
	address /= 0x1000;
	u32int tableIdx = address / 1024;
	if (tables[tableIdx] != 0) {
		return &(tables[tableIdx]->pages[address % 1024]);
	} else if (make) {
		u32int tmp;
		tables[tableIdx] = (page_table_t*)PageAlloc::alloc(&tmp);
		CMem::memset((u8int*)tables[tableIdx], 0, 0x1000);
		tablesPhysical[tableIdx] = tmp | 0x07;
		return &(tables[tableIdx]->pages[address % 1024]);
	} else {
		return 0;
	}
}

void PageDirectory::allocFrame(u32int address, bool is_user, bool is_writable) {
	page_t *p = getPage(address, true);
	if (p != 0) PhysMem::allocFrame(p, is_user, is_writable);
}

void PageDirectory::freeFrame(u32int address) {
	page_t *p = getPage(address, false);
	if (p == 0) return;
	PhysMem::freeFrame(p);
}

void PageDirectory::switchTo() {
	asm volatile("mov %0, %%cr3" :: "r"(physicalAddr));
	u32int cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
