#ifndef DEF_HEAP_CLASS_H
#define DEF_HEAP_CLASS_H

#include <common.h>
#include <Mutex.class.h>

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

#ifdef THIS_IS_MELON_KERNEL
class PageDirectory;
#else
#include <Binding/Process.class.h>
#endif

class Heap {
	private:
	u32int m_free, m_start, m_end;
	bool m_usable;
	heap_index_t m_index;
#ifdef THIS_IS_MELON_KERNEL
	bool m_user, m_rw;
	PageDirectory* m_pagedir;
#else
	Process m_process;
#endif

	Mutex m_mutex;

	void insertIntoIndex(heap_header_t *e);
	u32int findIndexEntry(heap_header_t *e);
	void removeFromIndex(u32int idx);
	void removeFromIndex(heap_header_t *e);

	void expand(u32int quantity);
	void contract();	//Quantity is automatically calculated
	
	public:
	Heap();
	~Heap();

#ifdef THIS_IS_MELON_KERNEL
	void create(u32int start, u32int size, u32int idxsize, PageDirectory* pagedir, bool user, bool rw);
#else
	void create(u32int start, u32int size, u32int idxsize);
#endif

	void* alloc(u32int sz, bool no_expand = false);
	void free(void* ptr);

	bool usable() { 
		m_mutex.waitLock();
		bool ret = m_usable;
		m_mutex.unlock();
		return ret;
	}

	u32int size() {
		m_mutex.waitLock();
		u32int ret = m_end - m_start;
		m_mutex.unlock();
		return ret;
	}

	u32int free() {
		m_mutex.waitLock();
		u32int ret = m_free;
		m_mutex.unlock();
	   	return ret;
	}
};


#endif
