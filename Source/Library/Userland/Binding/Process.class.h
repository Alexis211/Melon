#include <Syscall/RessourceCaller.class.h>

#include <Process.iface.h>

class Process : public RessourceCaller {
	public:
	static Process get() {
		u32int id = RessourceCaller::getObjId(PRIF_OBJTYPE);
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
};
