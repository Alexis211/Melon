#include "GDT.ns.h"

extern "C" void gdt_flush(u32int);

namespace GDT {

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;

void setGate(s32int num, u32int base, u32int limit, u8int access, u8int gran) {
	gdt_entries[num].base_low = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;
	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access = access;
}

void init() {
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base = (u32int)&gdt_entries;

	setGate(0, 0, 0, 0, 0);					//Null segment
	setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	//Kernel code segment
	setGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	//Kernel data segment
	setGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	//User code segment
	setGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	//User data segment

	gdt_flush((u32int)&gdt_ptr);
}

}
