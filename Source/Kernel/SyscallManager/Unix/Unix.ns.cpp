#include "Unix.ns.h"
#include <Unix.iface.h>
#include <TaskManager/Task.ns.h>

namespace Unix {

typedef int (*call)(u32int, u32int, u32int, u32int, u32int);

struct call_t {
	u32int id;
	call f;
} calls[] = {
	{UNIX_SC_EXIT, (call)uExit},
	{UNIX_SC_FORK, (call)uFork},
	{UNIX_SC_READ, (call)uRead},
	{UNIX_SC_WRITE, (call)uWrite},
	{UNIX_SC_OPEN, (call)uOpen},
	{UNIX_SC_CLOSE, (call)uClose},

	{UNIX_SC_SBRK, (call)uSbrk},
	{0, 0}
};


int handleCall(registers_t regs) {
	Sys::dumpRegs(&regs, *Task::currProcess()->getOutVT());
	u32int i = 0;
	while (calls[i].id != 0) {
		if (calls[i].id == regs.eax) {
			int ret = calls[i].f(regs.ebx, regs.ecx, regs.edx, regs.esi, regs.edi);
			return ret;
		}
		i++;
	}
	return -1;
}

}

int uExit(int errcode) {
	Task::currentThreadExits(E_EXIT);
	return 0;
}

int uFork() {
	return -1;
}

int uRead(int file, char *ptr, int len) {
	return 0;
}

int uWrite(int file, char *ptr, int len) {
	u8int b[4];
	for (int i = 0; i < 4; i++) {
		b[i] = ptr[len + i];
		ptr[len + i] = 0;
	}
	*(Task::currProcess()->getOutVT()) << ptr;
	for (int i = 0; i < 4; i++) ptr[len + i] = b[i];
	return len;
}

int uOpen(const char* name, int flags, int mode) {
	return -1;
}

int uClose(int file) {
	return 0;
}

int uSbrk(u32int start, u32int end) {
	if ((start & 0x00000FFF) != 0) start = (start & 0xFFFFF000);
	if ((end & 0xFFFFF000) + 0x1000 >= 0xC0000000) return -1;
	for (u32int i = start & 0xFFFFF000; i < end; i += 0x1000) {
		Task::currProcess()->getPagedir()->allocFrame(i, true, true);
	}
	return 0;
}
