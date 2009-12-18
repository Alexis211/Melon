#include "Unix.ns.h"
#include <Unix.iface.h>
#include <TaskManager/Task.ns.h>

#include <sys/stat.h>

#include "UnixInfo.class.h"

#include <VFS/VFS.ns.h>

#define INFO UnixInfo* info = Task::currProcess()->getUnixInfo();

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
	{UNIX_SC_WAITPID, (call)uWaitpid},
	{UNIX_SC_LINK, (call)uLink},
	{UNIX_SC_UNLINK, (call)uUnlink},
	{UNIX_SC_EXECVE, (call)uExecve},
	{UNIX_SC_STAT, (call)uStat},
	{UNIX_SC_LSEEK, (call)uLseek},
	{UNIX_SC_GETPID, (call)uGetpid},
	{UNIX_SC_FSTAT, (call)uFstat},
	{UNIX_SC_KILL, (call)uKill},
	{UNIX_SC_TIMES, (call)uTimes},
	{UNIX_SC_SBRK, (call)uSbrk},
	{0, 0}
};


int handleCall(registers_t regs) {
	//Sys::dumpRegs(&regs, *Task::currProcess()->getOutVT());
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

String realName(String file) {	//Translates a filename from Unix VFS to real VFS (empty string = does not exist)
	if (file.substr(0, 5) == "/dev/") return "";
	Vector<String> d = file.split("/");
	if (d[1] == "home") {
		if (d[2] == "Public") {
			return file.substr(5);	//  /home/Public/test => /Public/test
		} else {
			String ret = "/Private/";
			ret += d[2];
			ret += "/Unix";
			for (u32int i = 3; i < d.size(); i++) (ret += "/") += d[i];
			return ret;
		}
	}
	return String("/Unix") + file;
}

int stat(String file) {			//Get type of a file
	if (file.substr(0, 5) == "/dev/") {
		if (file == "/dev/stdout") return S_IFCHR;
		if (file == "/dev/stdin") return S_IFCHR;
		if (file == "/dev/stderr") return S_IFCHR;
	} else {
		String n = realName(file);
		if (!n.empty()) {
			FSNode* f = VFS::find(n);
			if (f != 0) {
				if (f->type() == NT_FILE) return S_IFREG;
				if (f->type() == NT_DIRECTORY) return S_IFDIR;
			}
		}
	}
	return 0;
}

}	//END NAMESPACE UNIX

//****************************
//								CALL HANDLERS
//													*************************************
int uExit(int errcode) {					//OK
	Task::currentThreadExits(E_EXIT);
	return 0;
}

int uFork() {								//TODO optionnal
	return -1;
}

int uRead(int file, char *ptr, int len) {	//OK
	INFO;
	UnixFileDescriptor* fd = info->getFd(file);
	if (fd == 0) return 0;
	return fd->read(ptr, len);
}

int uWrite(int file, char *ptr, int len) {	//OK
	INFO;
	UnixFileDescriptor* fd = info->getFd(file);
	if (fd == 0) return 0;
	return fd->write(ptr, len);
}

int uOpen(const char* name, int flags, int mode) {	//TODO
	return -1;
}

int uClose(int file) {						//OK
	INFO;
	info->closeFd(file);
	return 0;
}

int uWaitpid(int *status) {					//TODO
	return -1;
}

int uLink(char *old, char *nnew) {			//NOT TODO
	return -1;
}

int uUnlink(char *name) {					//OK
	String file = Unix::realName(name);
	FSNode* node = VFS::find(file);
	if (node == 0) return -1;
	if (!node->writable()) return -1;
	if (!VFS::remove(node)) return -1;
	return 0;
}

int uExecve(char *name, char **argv, char **env) {	//TODO optionnal
	return -1;
}

int uStat(const char *file) {				//OK
	return Unix::stat(file);
}

int uLseek(int file, int ptr, int dir) {	//OK
	INFO;
	UnixFileDescriptor* fd = info->getFd(file);
	if (fd == 0) return 0;
	return fd->lseek(ptr, dir);
}

int uGetpid() {								//OK
	return Task::currProcess()->getPid();
}

int uFstat(int file) {						//OK
	INFO;
	UnixFileDescriptor* fd = info->getFd(file);
	if (fd == 0) return 0;
	return Unix::stat(fd->getPath());
}

int uKill(int pid, int sig) {				//TODO optionnal
	return -1;
}

int uTimes() {								//NOT TODO
	return 0;
}

int uSbrk(u32int start, u32int end) {		//OK
	if ((start & 0x00000FFF) != 0) start = (start & 0xFFFFF000);
	if ((end & 0xFFFFF000) + 0x1000 >= 0xC0000000) return -1;
	for (u32int i = start & 0xFFFFF000; i < end; i += 0x1000) {
		Task::currProcess()->getPagedir()->allocFrame(i, true, true);
	}
	return 0;
}
