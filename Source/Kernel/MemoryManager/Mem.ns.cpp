#include <Core/common.wtf.h>
#include <MemoryManager/PhysMem.ns.h>

namespace Mem {

bool kheapUsable = false, pagingEnabled = false;
u32int placementAddress, kheapFree;

heap_index_t heapIndex;
u32int heapStart, heapEnd;

//Placement malloc, used while heap is not initialized
void *kallocInternal(u32int sz, bool align) {
	if (kheapUsable) return 0;
	if (align && (placementAddress & 0xFFFFF000)) {
		placementAddress &= 0xFFFFF000;
		placementAddress += 0x1000;
	}
	u32int temp = placementAddress;
	placementAddress += sz;
	for (u32int i = temp; i < placementAddress; i += 0x1000) {
		if (pagingEnabled) kernelPageDirectory->allocFrame(i, true, false);
	}
	return (void*)temp;
}

//***************************************************************************
//***                      HEAP INDEX FUNCTIONS                           ***
//***************************************************************************
void insertIntoHeapIndex(heap_header_t *e) {
	//If index is full, return
	if ((heapIndex.size * sizeof(heap_header_t*) + (u32int)heapIndex.data) >= heapStart) return;

	u32int iterator = 0;
	while (iterator < heapIndex.size && heapIndex.data[iterator]->size < e->size) {
		if (heapIndex.data[iterator] == e) return;
		iterator++;
	}
	if (iterator == heapIndex.size) {
		heapIndex.data[heapIndex.size++] = e;
	} else {
		u32int pos = iterator;
		iterator = heapIndex.size;
		while (iterator > pos) {
			heapIndex.data[iterator] = heapIndex.data[iterator - 1];
			iterator--;
		}
		heapIndex.size++;
		heapIndex.data[pos] = e;
	}
}

u32int heapIndexFindEntry(heap_header_t *e) {
	for (u32int  i = 0; i < heapIndex.size; i++) {
		if (heapIndex.data[i] == e)
			return i;
	}
	return (u32int) - 1;
}

void removeFromHeapIndex(u32int idx) {
	heapIndex.size--;
	while (idx < heapIndex.size) {
		heapIndex.data[idx] = heapIndex.data[idx + 1];
		idx++;
	}
}

void removeFromHeapIndex(heap_header_t *e) {
	u32int i = heapIndexFindEntry(e);
	if (i != (u32int) - 1) {
		removeFromHeapIndex(i);
	}
}

//***************************************************************************
//***                      MEMORY MANAGMENT FUNCTIONS                     ***
//***************************************************************************
void createHeap() {
	u32int heapIndexSize = PhysMem::total() * 64 + 0x10000;
	heapStart = placementAddress + 0x10000; //Set initial heap start
	heapEnd = heapStart + HEAP_MIN_SIZE + heapIndexSize;							//Set heap end

	//Alocate frames for the heap
	for (u32int i = placementAddress; i < heapEnd; i += 0x1000) {
		kernelPageDirectory->allocFrame(i, true, false);
	}

	heapIndex.data = (heap_header_t **)heapStart;		//Set index start
	heapIndex.size = 0;
	heapStart += heapIndexSize;	//Set some of available memory to be the index
	
	heap_header_t *hole = (heap_header_t*) heapStart;
	hole->size = (heapEnd - heapStart);
	hole->magic = HEAP_MAGIC;
	hole->is_hole = true;

	heap_footer_t *hole_footer = (heap_footer_t*) (heapEnd - sizeof(heap_footer_t));
	hole_footer->header = hole;
	hole_footer->magic = HEAP_MAGIC;

	insertIntoHeapIndex(hole);

	kheapUsable = true;
	kheapFree = (heapEnd - heapStart);
}

void expandHeap(u32int quantity) {
	if (quantity & 0x00000FFF)
		quantity = (quantity & 0xFFFFF000) + 0x1000;

	u32int newEnd = heapEnd + quantity;

	for (u32int i = heapEnd; i < newEnd; i++) {
		kernelPageDirectory->allocFrame(i, true, false);
	}

	heap_footer_t *last_footer = (heap_footer_t*) (heapEnd - sizeof(heap_footer_t));
	heap_header_t *last_header = last_footer->header;
	if (last_header->is_hole) {	//Last block of heap is a hole, update its size
		removeFromHeapIndex(last_header);
		last_header->size += quantity;

		last_footer = (heap_footer_t*) (newEnd - sizeof(heap_footer_t));

		last_footer->magic = HEAP_MAGIC;
		last_footer->header = last_header;

		insertIntoHeapIndex(last_header);
	} else {		//Last block is not a hole. Just add a new hole at the end
		last_header = (heap_header_t*)heapEnd;
		last_footer = (heap_footer_t*) (newEnd - sizeof(heap_footer_t));

		last_header->is_hole = true;
		last_header->magic = HEAP_MAGIC;
		last_header->size = quantity;
		
		last_footer->magic = HEAP_MAGIC;
		last_footer->header = last_header;

		insertIntoHeapIndex(last_header);
	}

	heapEnd = newEnd;
}

void contractHeap() {	//Automatically work out how much we can contract
	heap_footer_t *last_footer = (heap_footer_t*) (heapEnd - sizeof(heap_footer_t));
	heap_header_t *last_header = last_footer->header;
	if (last_header->is_hole == false) return;	//We need a hole at end of heap

	u32int quantity = 0;
	while ((heapEnd - heapStart) - quantity > HEAP_MIN_SIZE and 
			(last_header->size - quantity) > 0x1000)	//Always keep at least 0x1000 free at end
		quantity += 0x1000;
	if (quantity == 0) return;

	u32int newEnd = heapEnd - quantity;

	removeFromHeapIndex(last_header);
	last_header->size -= quantity;
	last_footer = (heap_footer_t*)((u32int)last_footer - quantity);
	last_footer->magic = HEAP_MAGIC;
	last_footer->header = last_header;
	insertIntoHeapIndex(last_header);

	for (u32int i = newEnd; i < heapEnd; i += 0x1000) {
		kernelPageDirectory->freeFrame(i);
	}

	heapEnd = newEnd;
}

void *kalloc(u32int sz, bool align) {
	if (!kheapUsable) return kallocInternal(sz, align);
	if (align) return 0;

	u32int newsize = sz + sizeof(heap_header_t) + sizeof(heap_footer_t);
	u32int iterator = 0;
	while (iterator < heapIndex.size) {
		if (heapIndex.data[iterator]->size >= newsize) break;
		iterator++;
	}
	if (iterator == heapIndex.size) {	//TODO : expand heap
		expandHeap((sz & 0xFFFFF000) + 0x1000);
		return kalloc(sz);	//Recurse call
	}

	heap_header_t *loc = heapIndex.data[iterator];
	heap_footer_t *footer = (heap_footer_t*)((u32int)loc + loc->size - sizeof(heap_footer_t));
	loc->is_hole = false;	//Update current header

	removeFromHeapIndex(loc);


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

		insertIntoHeapIndex(nextloc);
	}
	
	kheapFree -= loc->size;

	return (void*)((u32int)loc + sizeof(heap_header_t));
}

void kfree(void *ptr) { //TODO
	if (ptr == 0) return;

	heap_header_t *header = (heap_header_t*) ((u32int)ptr - sizeof(heap_header_t));
	heap_footer_t *footer = (heap_footer_t*)((u32int)header + header->size - sizeof(heap_footer_t));
	if (header->magic != HEAP_MAGIC or footer->magic != HEAP_MAGIC) return;

	kheapFree += header->size;

	//Unify left
	heap_footer_t *prev_footer = (heap_footer_t*)((u32int)header - sizeof(heap_footer_t));
	if (prev_footer->magic == HEAP_MAGIC && prev_footer->header->is_hole) {
		header = prev_footer->header;
		removeFromHeapIndex(header);

		footer->header = header;
		header->size = ((u32int)footer - (u32int)header + sizeof(heap_footer_t));
	}

	//Unify right
	heap_header_t *next_header = (heap_header_t*)((u32int)footer + sizeof(heap_footer_t));
	if (next_header->magic == HEAP_MAGIC && next_header->is_hole) {
		removeFromHeapIndex(next_header);
		footer = (heap_footer_t*)((u32int)footer + next_header->size);
		
		footer->header = header;
		header->size = ((u32int)footer - (u32int)header + sizeof(heap_footer_t));
	}

	header->is_hole = true;

	insertIntoHeapIndex(header);

	if ((u32int)footer == (heapEnd - sizeof(heap_footer_t)) and
			header->size >= 0x2000 and (heapEnd - heapStart > HEAP_MIN_SIZE)) {
		contractHeap();
	}
}

}
