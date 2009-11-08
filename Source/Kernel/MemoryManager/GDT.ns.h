#ifndef DEF_GDT_NS_H
#define DEF_GDT_NS_H

#include <common.h>

namespace GDT {
	struct gdt_entry_t {
		u16int limit_low;
		u16int base_low;
		u8int base_middle;
		u8int access;
		u8int granularity;
		u8int base_high;
	} __attribute__((packed));

	// A struct describing a Task State Segment.
	struct tss_entry_t {
		u32int prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
		u32int esp0;       // The stack pointer to load when we change to kernel mode.
		u32int ss0;        // The stack segment to load when we change to kernel mode.
		u32int esp1;       // Unused...
		u32int ss1;
		u32int esp2;
		u32int ss2;
		u32int cr3;
		u32int eip;
		u32int eflags;
		u32int eax;
		u32int ecx;
		u32int edx;
		u32int ebx;
		u32int esp;
		u32int ebp;
		u32int esi;
		u32int edi;
		u32int es;         // The value to load into ES when we change to kernel mode.
		u32int cs;         // The value to load into CS when we change to kernel mode.
		u32int ss;         // The value to load into SS when we change to kernel mode.
		u32int ds;         // The value to load into DS when we change to kernel mode.
		u32int fs;         // The value to load into FS when we change to kernel mode.
		u32int gs;         // The value to load into GS when we change to kernel mode.
		u32int ldt;        // Unused...
		u16int trap;
		u16int iomap_base;
		u8int ioports[8192];
	} __attribute__((packed));

	struct gdt_ptr_t {
		u16int limit;
		u32int base;
	} __attribute__((packed));

	void init();
	extern tss_entry_t tss_entry;	//Used for setting kernel stack
}

#endif
