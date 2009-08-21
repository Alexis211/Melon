#ifdef DEF_COMMON
//This must be included by common.wtf.h

#ifndef DEF_SYS_NS_H
#define DEF_SYS_NS_H

namespace Sys {
	void outb(u16int port, u8int value);
	u8int inb(u16int port);
	u16int inw(u16int port);
	void panic(char* message, char *file, u32int line);
	void panic_assert(char* file, u32int line, char *desc);
	void bochs_output(char* message, char *file, u32int line);
	void reboot();
}

#endif

#endif
