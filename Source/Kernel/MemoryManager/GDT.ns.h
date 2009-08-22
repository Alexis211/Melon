#ifndef DEF_GDT_NS_H
#define DEF_GDT_NS_H

#include <Core/common.wtf.h>

namespace GDT {
	struct gdt_entry_t {
		u16int limit_low;
		u16int base_low;
		u8int base_middle;
		u8int access;
		u8int granularity;
		u8int base_high;
	} __attribute__((packed));

	struct gdt_ptr_t {
		u16int limit;
		u32int base;
	} __attribute__((packed));

	void init();
}

#endif
