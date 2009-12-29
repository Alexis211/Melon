#include "IDT.ns.h"
#include <VTManager/SimpleVT.class.h>
#include <DeviceManager/Dev.ns.h>
#include <TaskManager/Task.ns.h>
#include <SyscallManager/Res.ns.h>

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

extern "C" void int64();		//Main syscall
extern "C" void int65();		//Syscall to request a task switch
extern "C" void int66();		//Syscall to signal that thread ended

extern "C" void idt_flush(u32int);

extern "C" void interrupt_handler(registers_t regs) {
	bool doSwitch = (regs.int_no == 32 or regs.int_no >= 65);	//SYSCALLS >= 65 are task-managing-related
	if (regs.int_no < 32) {				//************	HANDLE AN EXCEPTION
		if ((u32int)Task::currThread() == 0xFFFFFFFF or Task::currThread() == 0)
			PANIC_DUMP("Exception cannot be handled.", &regs);
		Task::currThread()->handleException(&regs, regs.int_no);
	} else if (regs.int_no < 48) {		//************	HANDLE AN IRQ
		if (regs.int_no >= 40)
			outb(0xA0, 0x20);
		outb(0x20, 0x20);
		asm volatile("sti");	//Make handling preemtible
		Dev::handleIRQ(regs, (regs.int_no - 32));
		asm volatile("cli");
		doSwitch = doSwitch or Task::IRQwakeup(regs.int_no - 32);
	} else if (regs.int_no == 64) {		//************	HANDLE A SYSCALL
		u32int res = (regs.eax >> 8);
		u8int wat = (regs.eax & 0xFF);
		if (res == 0xFFFFFF) {
			regs.eax = 0x01234567;		//RESERVED SYSCALL
		} else {
			asm volatile("sti");	//Make syscalls preemtible
			regs.eax = Res::call(res, wat, regs.ebx, regs.ecx, regs.edx, regs.edi, regs.esi);
			asm volatile("cli");
		}
		//Some syscalls have maybee modified current page directory, set it back to one for current process
		Task::currProcess()->getPagedir()->switchTo();
	} else if (regs.int_no == 66) {	//This syscall signals to kernel that thread ended.
		Task::currentThreadExits(regs.eax);	//DO NOT COUNT ON COMMING BACK FROM HERE
	}
	if (doSwitch) Task::doSwitch();	//DO NOT COUNT ON COMMING BACK FROM HERE EITHER
}

namespace IDT {

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void setGate(u8int num, u32int base, u16int sel, u8int flags) {
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[num].sel = sel;
	idt_entries[num].flags = flags | 0x60;
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
	setGate(65, (u32int)int65, 0x08, 0x8E);
	setGate(66, (u32int)int66, 0x08, 0x8E);
	
	idt_flush((u32int)&idt_ptr);
}

}
