#include <Syscall/RessourceCaller.class.h>

#include <Thread.iface.h>

class Thread : public RessourceCaller {
	public:
	static Thread get() {
		u32int id = RessourceCaller::sCall(THIF_OBJTYPE, THIF_SGETCTH);
		return Thread(id);
	}
	Thread(u32int id) : RessourceCaller(id, THIF_OBJTYPE) {}

	void sleep(u32int msecs) {
		doCall(THIF_SLEEP, msecs);
	}
	void finish(u32int errcode) {
		doCall(THIF_FINISH, errcode);
	}
};
