#include <Syscall/RessourceCaller.class.h>

#include <Process.iface.h>

class Process : public RessourceCaller {
	public:
	static Process get() {
		u32int id = RessourceCaller::getObjId(PR_IFACE_OBJTYPE);
		return Process(id);
	}
	Process(u32int id) : RessourceCaller(id, PR_IFACE_OBJTYPE) {}

	void exit() {
		doCall(PR_IFACE_EXIT);
	}
	void allocPage(u32int pos) {
		doCall(PR_IFACE_ALLOCPAGE, pos);
	}
	void freePage(u32int pos) {
		doCall(PR_IFACE_FREEPAGE, pos);
	}
};
