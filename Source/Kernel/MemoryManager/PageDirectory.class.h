#ifndef DEF_PAGEDIRECTORY_CLASS_H
#define DEF_PAGEDIRECTORY_CLASS_H

#include <common.h>
#include <Vector.class.h>
#include <MemoryManager/Segment.proto.h>

struct page_t {
	u32int present		: 1;
	u32int rw			: 1;
	u32int user			: 1;
	u32int accessed		: 1;
	u32int dirty		: 1;
	u32int unused		: 7;
	u32int frame		: 20;
};

struct page_table_t {
	page_t pages[1024];
};

struct PageDirectory {
	page_table_t *tables[1024];		//Logical address of each page tables
	u32int *tablesPhysical;			//Logical adress of the page directory = physical address of each page table
	u32int physicalAddr;			//Physical address of the page directory

	Vector<seg_map_t*> mappedSegs;

	//For mapping a page to a frame
	static void map(page_t *p, u32int frame, bool is_user, bool is_writable);
	static void unmap(page_t *p, bool physFree = false);

	static bool handlePageFault(u32int addr, bool write);

	PageDirectory();
	//PageDirectory(PageDirectory* other);	//Clones the other pagedir
	~PageDirectory();
	void switchTo();
	page_t *getPage(u32int address, bool make);

	//For mapping memory segments to the page directory
	void map(Segment* seg);
	void unmap(Segment* seg);

	private:
	PageDirectory(const PageDirectory& other);
	void operator=(const PageDirectory& other);

	static PageDirectory* currPD;
};



#endif
