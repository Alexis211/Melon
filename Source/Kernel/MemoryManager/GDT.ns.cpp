#include "GDT.ns.h"

extern "C" void gdt_flush(u32int);
extern "C" void tss_flush();

using namespace CMem;	//For memset

#define GDT_ENTRIES 6

namespace GDT {

gdt_entry_t gdt_entries[GDT_ENTRIES];
gdt_ptr_t   gdt_ptr;
tss_entry_t tss_entry;

void setGate(s32int num, u32int base, u32int limit, u8int access, u8int gran) {
	gdt_entries[num].base_low = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;
	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access = access;
}

void writeTSS(s32int num, u16int ss0, u32int esp0) {
	u32int base = (u32int)&tss_entry;
	u32int limit = base + sizeof(tss_entry);

	setGate(num, base, limit, 0xE9, 0x00);

	memset((u8int*)&tss_entry, 0, sizeof(tss_entry_t));

	tss_entry.ss0 = ss0;
	tss_entry.esp0 = esp0;
	tss_entry.iomap_base = sizeof(tss_entry_t) - 8192;

	tss_entry.cs = 0x0B;	//0x0B = Kernel code segment + RPL=3 (meaning it is supposed to be called from user mode)
	//0x13 = Kernel data segment + RPL=3 (meaning to be called from ring3)
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

void init() {
	gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
	gdt_ptr.base = (u32int)&gdt_entries;

	setGate(0, 0, 0, 0, 0);					//Null segment
	setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	//Kernel code segment
	setGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	//Kernel data segment
	setGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	//User code segment
	setGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	//User data segment
	writeTSS(5, 0x10, 0x0);

	gdt_flush((u32int)&gdt_ptr);
	tss_flush();
}

}
