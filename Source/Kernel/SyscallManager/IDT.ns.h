#ifndef DEF_IDT_NS_H
#define DEF_IDT_NS_H

#include <Core/common.wtf.h>

struct registers_t {
	u32int ds;                  // Data segment selector
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
	u32int int_no, err_code;    // Interrupt number and error code (if applicable)
	u32int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

namespace IDT {
	struct idt_entry_t {
		u16int base_lo;
		u16int sel;
		u8int always0;
		u8int flags;
		u16int base_hi;
	}__attribute__((packed));

	struct idt_ptr_t {
		u16int limit;
		u32int base;
	}__attribute__((packed));

	void init();
}

#endif

