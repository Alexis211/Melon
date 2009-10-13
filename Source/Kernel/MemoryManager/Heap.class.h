#ifndef DEF_HEAP_CLASS_H
#define DEF_HEAP_CLASS_H

#include <Core/common.wtf.h>

//Heap minimum size : 2M
#define HEAP_MIN_SIZE 0x00200000
//Heap magic number, for verifications
#define HEAP_MAGIC 0xBEEF1337

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

class PageDirectory;

class Heap {
	private:
	bool m_usable, m_user, m_rw;
	u32int m_free, m_start, m_end;
	heap_index_t m_index;
	PageDirectory* m_pagedir;

	void insertIntoIndex(heap_header_t *e);
	u32int findIndexEntry(heap_header_t *e);
	void removeFromIndex(u32int idx);
	void removeFromIndex(heap_header_t *e);

	void expand(u32int quantity);
	void contract();	//Quantity is automatically calculated
	

	public:
	Heap();
	~Heap();

	bool usable() { return m_usable; }

	void create(u32int start, u32int size, u32int idxsize, PageDirectory* pagedir, bool user, bool rw);

	void* alloc(u32int sz, bool no_expand = false);
	void free(void* ptr);

	u32int size() { return m_end - m_start; }
	u32int free() { return m_free; }
};


#endif
