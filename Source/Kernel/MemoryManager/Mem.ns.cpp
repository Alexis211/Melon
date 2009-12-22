#include <common.h>
#include <MemoryManager/PhysMem.ns.h>
#include <TaskManager/Task.ns.h>
#include <Heap.class.h>
#include <VTManager/SimpleVT.class.h>

namespace Mem {

bool pagingEnabled = false;
size_t placementAddress;

Heap kheap;

//Placement malloc, used while heap is not initialized
void *kallocInternal(size_t sz, bool align) {
	if (kheap.usable()) return 0;
	if (align && (placementAddress & 0xFFFFF000)) {
		placementAddress &= 0xFFFFF000;
		placementAddress += 0x1000;
	}
	u32int temp = placementAddress;
	placementAddress += sz;
	for (u32int i = temp; i < placementAddress; i += 0x1000) {
		if (pagingEnabled) PhysMem::keSeg.allocFrame(i);
	}
	return (void*)temp;
}

//***************************************************************************
//***                      MEMORY MANAGMENT FUNCTIONS                     ***
//***************************************************************************
void createHeap() {
	u32int heapIndexSize = PhysMem::total() * 64 + 0x10000;
	u32int heapStart = (placementAddress & 0xFFFFF000) + 0x10000; //Set initial heap start
	u32int heapSize = HEAP_MIN_SIZE + heapIndexSize;							//Calculate heap size

	for (u32int i = (placementAddress & 0xFFFFF000); i < heapStart; i += 0x1000) {
		PhysMem::keSeg.allocFrame(i);
	}

	kheap.create(heapStart, heapSize, heapIndexSize, &PhysMem::keSeg);
}

void *alloc(size_t sz, bool align) {
	if (!kheap.usable()) return kallocInternal(sz, align);
	if (align) return 0;

	return kheap.alloc(sz);
}

void free(void *ptr) {
	kheap.free(ptr);
}

void* mkXchgSpace(size_t sz) {
	return Task::currThread()->mkXchgSpace(sz);
}

u32int kheapSize() {
	return kheap.size();
}

u32int kheapFree() {
	return kheap.free();
}

}
