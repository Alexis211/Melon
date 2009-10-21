#include <Syscall/RessourceCaller.class.h>

#include <Process.iface.h>
#include <String.class.h>

class Process : public RessourceCaller {
	public:
	static Process get() {
		u32int id = RessourceCaller::sCall(PRIF_OBJTYPE, PRIF_SGETCPR);
		return Process(id);
	}
	Process(u32int id) : RessourceCaller(id, PRIF_OBJTYPE) {}

	void exit() {
		doCall(PRIF_EXIT);
	}
	void allocPage(u32int pos) {
		doCall(PRIF_ALLOCPAGE, pos);
	}
	void freePage(u32int pos) {
		doCall(PRIF_FREEPAGE, pos);
	}
	u32int getPid() {
		return doCall(PRIF_GETPID);
	}
	u32int getPpid() {
		return doCall(PRIF_GETPPID);
	}
	String getCmdline() {
		return String::unserialize(doCall(PRIF_GETCMDLINE));
	}
};
