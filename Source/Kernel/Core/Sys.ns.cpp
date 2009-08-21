//This automatically includes Sys.ns.h
#include <Core/common.wtf.h>
#include <VTManager/VirtualTerminal.class.h>

#define DEBUGVT(x) VirtualTerminal *x = new VirtualTerminal(5, 46, 0, 15); x->map(); x->put('\n');

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

//TODO : make PANIC output a visible message

//Used by PANIC() macro (see common.wtf.h)
void panic(char *message, char *file, u32int line) {
	asm volatile("cli");

	DEBUGVT(vt);
	bochs_output("PANIC : ", file, line);
	bochs_output(message, file, 0);
	*vt << "  PANIC : " << message << "\n    In " << file << ":" << (s32int)line << "\n";

	while (1) asm volatile("hlt"); //Enter infinite loop for halt
}

//Used by ASSERT() macro (see common.wtf.h)
void panic_assert(char *file, u32int line, char *desc) {
	asm volatile("cli");

	DEBUGVT(vt);
	bochs_output("ASSERTION FAILED : ", file, line);
	bochs_output(desc, file, 0);
	*vt << "  ASSERTION FAILED : " << desc << "\n    In " << file << ":" << (s32int)line << "\n";

	while (1) asm volatile("hlt"); //Enter infinite loop for halt
}

void reboot() {
	outb(0x64, 0xFE);
}

}
