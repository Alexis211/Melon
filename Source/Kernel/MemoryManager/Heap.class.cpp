#include "Heap.class.h"
#include <MemoryManager/PageDirectory.class.h>

Heap::Heap() {
	m_usable = false;
	m_index.data = 0;
	m_index.size = 0;
}

Heap::~Heap() {
	//TODO (optionnal) : free pages.
}

void Heap::create(u32int start, u32int size, u32int idxsize, PageDirectory* pagedir, bool user, bool rw) {
	if (start & 0x0FFF) start = (start & 0xFFFFF000) + 0x1000;
	if (size & 0x0FFF) size = (size & 0xFFFFF000) + 0x1000;
	m_start = start + idxsize;	//m_start is start of real data, start is start of index.
	m_end = start + size;

	m_pagedir = pagedir;
	m_user = user;
	m_rw = rw;
	
	//Allocate frames for heap
	for (u32int i = start ; i < m_end; i += 0x1000) {
		m_pagedir->allocFrame(i, m_user, m_rw);
	}

	m_index.data = (heap_header_t **)start;		//Set index start. start == start of all heap
	m_index.size = 0;

	heap_header_t *hole = (heap_header_t*) m_start;	//m_start == start of data
	hole->size = (m_end - m_start);
	hole->magic = HEAP_MAGIC;
	hole->is_hole = true;

	heap_footer_t *hole_footer = (heap_footer_t*) (m_end - sizeof(heap_footer_t));
	hole_footer->header = hole;
	hole_footer->magic = HEAP_MAGIC;

	insertIntoIndex(hole);

	m_usable = true;
	m_free = (m_end - m_start);
}

void Heap::expand(u32int quantity) {
	if (quantity & 0x00000FFF)
		quantity = (quantity & 0xFFFFF000) + 0x1000;

	u32int newEnd = m_end + quantity;

	for (u32int i = m_end; i < newEnd; i++) {
		m_pagedir->allocFrame(i, m_user, m_rw);
	}

	heap_footer_t *last_footer = (heap_footer_t*) (m_end - sizeof(heap_footer_t));
	heap_header_t *last_header = last_footer->header;
	if (last_header->is_hole) {	//Last block of heap is a hole, update its size
		removeFromIndex(last_header);
		last_header->size += quantity;

		last_footer = (heap_footer_t*) (newEnd - sizeof(heap_footer_t));

		last_footer->magic = HEAP_MAGIC;
		last_footer->header = last_header;

		insertIntoIndex(last_header);
	} else {		//Last block is not a hole. Just add a new hole at the end
		last_header = (heap_header_t*)m_end;
		last_footer = (heap_footer_t*) (newEnd - sizeof(heap_footer_t));

		last_header->is_hole = true;
		last_header->magic = HEAP_MAGIC;
		last_header->size = quantity;
		
		last_footer->magic = HEAP_MAGIC;
		last_footer->header = last_header;

		insertIntoIndex(last_header);
	}

	m_end = newEnd;
	m_free += quantity;
}

void Heap::contract() {	//Automatically work out how much we can contract
	heap_footer_t *last_footer = (heap_footer_t*) (m_end - sizeof(heap_footer_t));
	heap_header_t *last_header = last_footer->header;
	if (last_header->is_hole == false) return;	//We need a hole at end of heap

	u32int quantity = 0;
	while ((m_end - m_start) - quantity > HEAP_MIN_SIZE and 
			(last_header->size - quantity) > 0x1000)	//Always keep at least 0x1000 free at end
		quantity += 0x1000;
	if (quantity == 0) return;

	u32int newEnd = m_end - quantity;
	m_free -= quantity;

	removeFromIndex(last_header);
	last_header->size -= quantity;
	last_footer = (heap_footer_t*)((u32int)last_footer - quantity);
	last_footer->magic = HEAP_MAGIC;
	last_footer->header = last_header;
	insertIntoIndex(last_header);

	for (u32int i = newEnd; i < m_end; i += 0x1000) {
		m_pagedir->freeFrame(i);
	}

	m_end = newEnd;
}

void *Heap::alloc(u32int sz, bool no_expand) {
	u32int newsize = sz + sizeof(heap_header_t) + sizeof(heap_footer_t);
	u32int iterator = 0;
	while (iterator < m_index.size) {
		if (m_index.data[iterator]->size >= newsize) break;
		iterator++;
	}
	if (iterator == m_index.size) { //No hole is big enough
		if (no_expand) return 0;
		expand((sz & 0xFFFFF000) + 0x1000);
		return alloc(sz, true);	//Recurse call
	}

	heap_header_t *loc = m_index.data[iterator];
	heap_footer_t *footer = (heap_footer_t*)((u32int)loc + loc->size - sizeof(heap_footer_t));
	loc->is_hole = false;	//Update current header

	removeFromIndex(loc);

		//Here we create a new hole after currently allocated block, but only if we have enough space. If we don't, we simply allocate a bigger block so that we don't loose space
	if (loc->size > (newsize + sizeof(heap_header_t) + sizeof(heap_footer_t))) { 
		loc->size = newsize;	//Update header for return block

		heap_footer_t *newfooter = (heap_footer_t*)((u32int)loc + newsize - sizeof(heap_footer_t)); //Write footer for return block
		newfooter->header = loc;
		newfooter->magic = HEAP_MAGIC;

		heap_header_t *nextloc = (heap_header_t*)((u32int)loc + newsize); //Write header for new hole
		nextloc->is_hole = true;
		nextloc->magic = HEAP_MAGIC;
		nextloc->size = ((u32int)footer - (u32int)nextloc + sizeof(heap_footer_t));

		footer->header = nextloc;	//Write footer for new hole
		footer->magic = HEAP_MAGIC;

		insertIntoIndex(nextloc);
	}
	
	m_free -= loc->size;

	return (void*)((u32int)loc + sizeof(heap_header_t));
}

void Heap::free(void *ptr) {
	if (ptr == 0) return;

	heap_header_t *header = (heap_header_t*) ((u32int)ptr - sizeof(heap_header_t));
	heap_footer_t *footer = (heap_footer_t*)((u32int)header + header->size - sizeof(heap_footer_t));
	if (header->magic != HEAP_MAGIC or footer->magic != HEAP_MAGIC) return;

	m_free += header->size;

	//Unify left
	heap_footer_t *prev_footer = (heap_footer_t*)((u32int)header - sizeof(heap_footer_t));
	if (prev_footer->magic == HEAP_MAGIC && prev_footer->header->is_hole) {
		header = prev_footer->header;
		removeFromIndex(header);

		footer->header = header;
		header->size = ((u32int)footer - (u32int)header + sizeof(heap_footer_t));
	}

	//Unify right
	heap_header_t *next_header = (heap_header_t*)((u32int)footer + sizeof(heap_footer_t));
	if (next_header->magic == HEAP_MAGIC && next_header->is_hole) {
		removeFromIndex(next_header);
		footer = (heap_footer_t*)((u32int)footer + next_header->size);
		
		footer->header = header;
		header->size = ((u32int)footer - (u32int)header + sizeof(heap_footer_t));
	}

	header->is_hole = true;

	insertIntoIndex(header);

	if ((u32int)footer == (m_end - sizeof(heap_footer_t)) and
			header->size >= 0x2000 and (m_end - m_start > HEAP_MIN_SIZE)) {
		contract();
	}
}
