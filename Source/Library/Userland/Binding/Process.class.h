#ifndef DEF_PROCESS_CLASS_H
#define DEF_PROCESS_CLASS_H

#include <Syscall/RessourceCaller.class.h>
#include <Binding/VirtualTerminal.class.h>

#include <Process.iface.h>
#include <String.class.h>

class Process : public RessourceCaller {
	public:
	static Process get() {
		u32int id = RessourceCaller::sCall(PRIF_OBJTYPE, PRIF_SGETCPR);
		return Process(id);
	}
	static Process run(const String& app) {
		return Process(sCall(PRIF_OBJTYPE, PRIF_SRUN, (u32int)&app));
	}
	Process(u32int id) : RessourceCaller(id, PRIF_OBJTYPE) {}

	void exit() {
		doCall(PRIF_EXIT);
	}
	void allocPages(u32int pos, u32int count) {
		doCall(PRIF_ALLOCPAGES, pos, count);
	}
	void freePages(u32int pos, u32int count) {
		doCall(PRIF_FREEPAGES, pos, count);
	}
	u32int getPid() {
		return doCall(PRIF_GETPID);
	}
	u32int getPpid() {
		return doCall(PRIF_GETPPID);
	}
	u32int getUid() {
		return doCall(PRIF_GETUID);
	}
	u32int argc() {
		return doCall(PRIF_ARGC);
	}
	String argv(u32int idx) {
		return String::unserialize(doCall(PRIF_ARGV, idx));
	}
	void start() {
		doCall(PRIF_START);
	}
	s32int wait() {
		return sCall(PRIF_OBJTYPE, PRIF_SWAIT, resId());
	}
	void autoDelete(bool ad = true) {
		doCall(PRIF_AUTODELETE, (ad ? 1 : 0));
	}
	void pushArg(const String& arg) {
		doCall(PRIF_PUSHARG, (u32int)&arg);
	}
	void setInVT(VirtualTerminal vt) {
		doCall(PRIF_SETINVT, vt.resId());
	}
	void setOutVT(VirtualTerminal vt) {
		doCall(PRIF_SETOUTVT, vt.resId());
	}
	bool authenticatePW(String user, String pw) {
		return (doCall(PRIF_AUTHPW, (u32int)&user, (u32int)&pw) == 1);
	}
	bool authenticateNoPW(String user) {
		return (doCall(PRIF_AUTHNOPW, (u32int)&user) == 1);
	}
};

#endif
