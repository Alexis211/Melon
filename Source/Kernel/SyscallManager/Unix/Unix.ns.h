#ifndef DEF_UNIX_NS_H
#define DEF_UNIX_NS_H

#include <SyscallManager/IDT.ns.h>
#include <Unix.iface.h>

namespace Unix {
	int handleCall(registers_t regs);
}

extern "C" {
	int uExit(int errcode);
	int uFork();
	int uRead(int file, char *ptr, int len);
	int uWrite(int file, char *ptr, int len);
	int uOpen(const char* name, int flags, int mode);
	int uClose(int file);
	int uWaitpid(int *status);
	int uLink(char *old, char *nnew);
	int uUnlink(char *name);
	int uExecve(char *name, char **argv, char **env);
	int uStat(const char *file);
	int uLseek(int file, int ptr, int dir);
	int uGetpid();
	int uFstat(int file);
	int uKill(int pid, int sig);
	int uTimes();
	int uSbrk(u32int start, u32int end);
}

#endif
