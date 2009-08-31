#ifdef DEF_COMMON

#ifndef DEF_MEM_NS_H
#define DEF_MEM_NS_H

//Heap minimum size : 2M
#define HEAP_MIN_SIZE 0x00200000
//Heap magic number, for verifications
#define HEAP_MAGIC 0xBEEF1337

namespace Mem {
	extern bool pagingEnabled;
	extern u32int placementAddress, kheapFree;

	struct heap_header_t {
		u32int magic;
		bool is_hole;
		u32int size;
	};

	struct heap_footer_t {
		u32int magic;
		heap_header_t *header;
	};

	struct heap_index_t {
		heap_header_t **data;
		u32int size;
	};

	void createHeap();
	void *kalloc(u32int sz, bool align = false);
	void kfree(void *ptr);

	u32int kheapSize();
}

#endif

#endif
