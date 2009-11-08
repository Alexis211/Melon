#ifdef DEF_COMMON
//This must be included by common.wtf.h

#ifndef DEF_SYS_NS_H
#define DEF_SYS_NS_H

#define PANIC(msg) Sys::panic(msg, __FILE__, __LINE__)
#define PANIC_DUMP(msg, regs) Sys::panic(msg, regs, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : Sys::panic_assert(__FILE__, __LINE__, #b))

#ifdef OPT_DEBUG
#define DEBUG(m) Sys::bochs_output(m, __FILE__, __LINE__)
#define DEBUG_HEX(m) Sys::bochs_output_hex(m);
#else
#define DEBUG(m)
#define DEBUG_HEX(m)
#endif

//This file contains system-relative functions
class String;
class VirtualTerminal;
struct registers_t;

namespace Sys {
	void outb(u16int port, u8int value);
	u8int inb(u16int port);
	u16int inw(u16int port);
	void dumpRegs(registers_t *regs, VirtualTerminal& vt);
	void panic(char* message, char *file, u32int line);
	void panic(char* message, registers_t *regs, char *file, u32int line);
	void panic_assert(char* file, u32int line, char *desc);
	void bochs_output(char* message, char *file, u32int line);
	void bochs_output(String message, char *file, u32int line);
	void bochs_output_hex(u32int i);
	void reboot();
	void halt();
	u32int scall(u8int, u32int, u32int, u32int, u32int);	//System call handler
}

#endif

#endif
