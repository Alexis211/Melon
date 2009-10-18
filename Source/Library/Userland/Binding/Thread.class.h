#include <Syscall/RessourceCaller.class.h>

#include <Thread.iface.h>

class Thread : public RessourceCaller {
	public:
	static Thread get() {
		u32int id = RessourceCaller::getObjId(TH_IFACE_OBJTYPE);
		return Thread(id);
	}
	Thread(u32int id) : RessourceCaller(id, TH_IFACE_OBJTYPE) {}

	void sleep(u32int msecs) {
		doCall(TH_IFACE_SLEEP, msecs);
	}
	void finish(u32int errcode) {
		doCall(TH_IFACE_FINISH, errcode);
	}
};
