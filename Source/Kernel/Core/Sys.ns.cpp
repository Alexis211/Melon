//This automatically includes Sys.ns.h
#include <common.h>
#include <Core/Log.ns.h>
#include <VTManager/SimpleVT.class.h>
#include <SyscallManager/IDT.ns.h>
#include <Sys.iface.h>
#include <UserManager/Usr.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <DeviceManager/Time.ns.h>

#define DEBUGVT(x) SimpleVT *x = new SimpleVT(4, 56, 0, 15); x->map(); x->put('\n');

using namespace CMem;

namespace Sys {

void outb (u16int port, u8int value) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8int inb (u16int port) {
	u8int ret;
	asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

u16int inw (u16int port) {
	u16int ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

//Used by DEBUG() macro (see common.wtf.h)
void bochs_output(char *message, char *file, u32int line) {
	for (u32int i = 0; i < strlen(message); i++) {
		outb(0xE9, message[i]);
	}
	char* t = (char*)" (in ";
	for (u32int i = 0; i < strlen(t); i++) {
		outb(0xE9, t[i]);
	}
	for (u32int i = 0; i < strlen(file); i++) {
		outb(0xE9, file[i]);
	}
	outb(0xE9, ')');
	outb(0xE9, '\n');
} 

void bochs_output(String message, char *file, u32int line) {
	for (u32int i = 0; i < message.size(); i++) {
		outb(0xE9, message[i].toAscii());
	}
	char* t = (char*)" (in ";
	for (u32int i = 0; i < strlen(t); i++) {
		outb(0xE9, t[i]);
	}
	for (u32int i = 0; i < strlen(file); i++) {
		outb(0xE9, file[i]);
	}
	outb(0xE9, ')');
	outb(0xE9, '\n');
} 

void bochs_output_hex(u32int i) {
	char hexdigits[] = "0123456789ABCDEF";
	outb(0xE9, '0');
	outb(0xE9, 'x');
	for (u32int j = 0; j < 8; j++) {
		outb(0xE9, hexdigits[(i & 0xF0000000) >> 28]);
		i = i << 4;
	}
}

void dumpRegs(registers_t *regs, VirtualTerminal& vt) {
	vt << "ds=" << (u32int)regs->ds << ", eip=" << (u32int)regs->eip << ", cs=" << (u32int)regs->cs << "\n";
	vt << "edi=" << (u32int)regs->edi << ", esi=" << (u32int)regs->esi << ", ebp=" << (u32int)regs->ebp <<
		", esp=" << (u32int)regs->esp << "\n";
	vt << "eax=" << (u32int)regs->eax << ", ebx=" << (u32int)regs->ebx << ", ecx=" << (u32int)regs->ecx <<
		", edx=" << (u32int)regs->edx << "\n";
	vt << "int_no=" << (s32int)regs->int_no << ", err_code=" << (u32int)regs->err_code << "\n";
	vt << "eflags=" << (u32int)regs->eflags << ", useresp=" << (u32int)regs->useresp << ", ss=" << (u32int)regs->ss << "\n";
}

//Used by PANIC() macro (see common.wtf.h)
void panic(char *message, char *file, u32int line) {
	asm volatile("cli");

	DEBUGVT(vt);
	bochs_output("PANIC : ", file, line);
	bochs_output(message, file, 0);
	*vt << "  PANIC : " << message << "\n    In " << file << ":" << (s32int)line;

	while (1) asm volatile("hlt"); //Enter infinite loop for halt
}

void panic(char *message, registers_t *regs, char *file, u32int line) {
	asm volatile("cli");

	SimpleVT vt(12, 70, 7, 1);
	vt.map();
	vt.write("\n");

	vt << "PANIC : " << message << "\n => in " << file << " at " << (s32int)line << "\n\n";
	dumpRegs(regs, vt);

	if (regs->int_no == 14) {
		u32int cr2;
		asm volatile("mov %%cr2, %0" : "=r"(cr2));
		vt << "cr2=" << (u32int)cr2 << "\n";
	}
	vt << "\n\n";

	while (1) asm volatile("cli; hlt");

	u32int *v = (u32int*)regs->ebp;
	for (int i = 0; i < 10; i++) {
		vt << "ebp=" << (u32int)v << ", value=" << (u32int)(v[1]) << "\n";
		v = (u32int*)(*v);
	}
}

//Used by ASSERT() macro (see common.wtf.h)
void panic_assert(char *file, u32int line, char *desc) {
	asm volatile("cli");

	DEBUGVT(vt);
	bochs_output("ASSERTION FAILED : ", file, line);
	bochs_output(desc, file, 0);
	*vt << "  ASSERTION FAILED : " << desc << "\n    In " << file << ":" << (s32int)line;

	while (1) asm volatile("hlt"); //Enter infinite loop for halt
}

void shutdown_cleanup() {
	asm volatile("cli");
	Log::close();
}

void reboot() {
	shutdown_cleanup();
	outb(0x64, 0xFE);
}

void halt() {
	shutdown_cleanup();
	String message("MELON SEZ : KTHXBYE, U CAN NAOW TURNZ OFF UR COMPUTER.");
	SimpleVT vt(3, message.size() + 16, 7, 6);
	vt.map();
	vt << "\n\t" << message;
	while (1) asm volatile("cli; hlt");
}

u32int scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == SYIF_HALT && ISROOT) halt();
	if (wat == SYIF_REBOOT && ISROOT) reboot();
	if (wat == SYIF_UPTIME) return Time::uptime();
	if (wat == SYIF_TOTALRAM) return PhysMem::total() * 4;
	if (wat == SYIF_FREERAM) return PhysMem::free() * 4;
	return (u32int) - 1;
}

}
