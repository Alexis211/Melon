#ifndef DEF_PAGEDIRECTORY_CLASS_H
#define DEF_PAGEDIRECTORY_CLASS_H

#include <Core/common.wtf.h>

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
	page_table_t *tables[1024];	
	u32int *tablesPhysical;
	u32int physicalAddr;

	PageDirectory();
	~PageDirectory();
	page_t *getPage(u32int address, bool make);
	void allocFrame(u32int address, bool is_user, bool is_writable);
	void freeFrame(u32int address);
	void switchTo();
};



#endif
