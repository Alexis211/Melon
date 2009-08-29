#include "IDT.ns.h"
#include <VTManager/VirtualTerminal.class.h>
#include <DeviceManager/Dev.ns.h>
#include <TaskManager/Task.ns.h>

using namespace Sys; 	//For outb

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

extern "C" void int64();

extern "C" void idt_flush(u32int);

extern "C" void interrupt_handler(registers_t regs) {
	bool doSwitch = (regs.int_no == 32 or regs.int_no == 64);
	if (regs.int_no < 32) {
		IDT::handleException(regs, regs.int_no);
	} else if (regs.int_no < 48) {
		if (regs.int_no >= 40)
			outb(0xA0, 0x20);
		outb(0x20, 0x20);
		Dev::handleIRQ(regs, (regs.int_no - 32));
		doSwitch = doSwitch or Task::IRQwakeup(regs.int_no - 32);
	}
	if (doSwitch) Task::doSwitch();
}

namespace IDT {

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void setGate(u8int num, u32int base, u16int sel, u8int flags) {
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[num].sel = sel;
	idt_entries[num].flags = flags;
	idt_entries[num].always0 = 0;
}

void init() {
	idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
	idt_ptr.base = (u32int)&idt_entries;

	CMem::memset((u8int*)&idt_entries, 0, sizeof(idt_entry_t) * 256);

	//Remap the IRQ table
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	setGate(0, (u32int)isr0, 0x08, 0x8E);
	setGate(1, (u32int)isr1, 0x08, 0x8E);
	setGate(2, (u32int)isr2, 0x08, 0x8E);
	setGate(3, (u32int)isr3, 0x08, 0x8E);
	setGate(4, (u32int)isr4, 0x08, 0x8E);
	setGate(5, (u32int)isr5, 0x08, 0x8E);
	setGate(6, (u32int)isr6, 0x08, 0x8E);
	setGate(7, (u32int)isr7, 0x08, 0x8E);
	setGate(8, (u32int)isr8, 0x08, 0x8E);
	setGate(9, (u32int)isr9, 0x08, 0x8E);
	setGate(10, (u32int)isr10, 0x08, 0x8E);
	setGate(11, (u32int)isr11, 0x08, 0x8E);
	setGate(12, (u32int)isr12, 0x08, 0x8E);
	setGate(13, (u32int)isr13, 0x08, 0x8E);
	setGate(14, (u32int)isr14, 0x08, 0x8E);
	setGate(15, (u32int)isr15, 0x08, 0x8E);
	setGate(16, (u32int)isr16, 0x08, 0x8E);
	setGate(17, (u32int)isr17, 0x08, 0x8E);
	setGate(18, (u32int)isr18, 0x08, 0x8E);
	setGate(19, (u32int)isr19, 0x08, 0x8E);
	setGate(20, (u32int)isr20, 0x08, 0x8E);
	setGate(21, (u32int)isr21, 0x08, 0x8E);
	setGate(22, (u32int)isr22, 0x08, 0x8E);
	setGate(23, (u32int)isr23, 0x08, 0x8E);
	setGate(24, (u32int)isr24, 0x08, 0x8E);
	setGate(25, (u32int)isr25, 0x08, 0x8E);
	setGate(26, (u32int)isr26, 0x08, 0x8E);
	setGate(27, (u32int)isr27, 0x08, 0x8E);
	setGate(28, (u32int)isr28, 0x08, 0x8E);
	setGate(29, (u32int)isr29, 0x08, 0x8E);
	setGate(30, (u32int)isr30, 0x08, 0x8E);
	setGate(31, (u32int)isr31, 0x08, 0x8E);

	setGate(32, (u32int)irq0, 0x08, 0x8E);
	setGate(33, (u32int)irq1, 0x08, 0x8E);
	setGate(34, (u32int)irq2, 0x08, 0x8E);
	setGate(35, (u32int)irq3, 0x08, 0x8E);
	setGate(36, (u32int)irq4, 0x08, 0x8E);
	setGate(37, (u32int)irq5, 0x08, 0x8E);
	setGate(38, (u32int)irq6, 0x08, 0x8E);
	setGate(39, (u32int)irq7, 0x08, 0x8E);
	setGate(40, (u32int)irq8, 0x08, 0x8E);
	setGate(41, (u32int)irq9, 0x08, 0x8E);
	setGate(42, (u32int)irq10, 0x08, 0x8E);
	setGate(43, (u32int)irq11, 0x08, 0x8E);
	setGate(44, (u32int)irq12, 0x08, 0x8E);
	setGate(45, (u32int)irq13, 0x08, 0x8E);
	setGate(46, (u32int)irq14, 0x08, 0x8E);
	setGate(47, (u32int)irq15, 0x08, 0x8E);

	setGate(64, (u32int)int64, 0x08, 0x8E);
	
	idt_flush((u32int)&idt_ptr);
}

void handleException(registers_t regs, int no) {
	asm volatile("cli;");
	char* exceptions[] = {
		"Division by zero", "Debug exception", "Non maskable interrupt", 
		"Breakpoint exception", "'Into detected overflow'", "Out of bounds exception",
		"Invalid opcode exception", "No coprocessor exception", "Double fault",
		"Coprocessor segment overrun", "Bad TSS", "Segment not present",
		"Stack fault", "General protection fault", "Page fault",
		"Unknown", "Coprocessor fault", "Alignement check exception",
		"Machine check exception", "", "",
		"", "", "",
		"", "", "",
		"", "", "",
		"", ""};

	VirtualTerminal *vt = new VirtualTerminal(5, 50, 0, 15);
	vt->map();

	*vt << "\n  Unhandled exception " << (s32int)no << " at " << (u32int)regs.cs << ":" <<
	   	(u32int)regs.eip << "\n  :: " << exceptions[no];

	if (no == 14) {	//Page fault
		int present = !(regs.err_code & 0x1);
		int rw = regs.err_code & 0x2;
		int us = regs.err_code & 0x4;
		int rsvd = regs.err_code & 0x8;
		u32int faddr;
		asm volatile("mov %%cr2, %0" : "=r"(faddr));
		*vt << "\n   ";
		if (present) *vt << "Present ";
		if (rw) *vt << "R/W ";
		if (us) *vt << "User ";
		if (rsvd) *vt << "Rsvd ";
		*vt << "At:" << (u32int)faddr;
	}

	asm volatile("hlt");
}

}
