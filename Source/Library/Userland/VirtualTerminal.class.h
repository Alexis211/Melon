#include <Syscall/RessourceCaller.class.h>

#include <VirtualTerminal.iface.h>

#include <WChar.class.h>

class VirtualTerminal : public RessourceCaller {
	public:
	static VirtualTerminal get() {
		u32int id = RessourceCaller::getObjId(VT_IFACE_OBJTYPE);
		return VirtualTerminal(id);
	}
	VirtualTerminal(u32int id) : RessourceCaller(id, VT_IFACE_OBJTYPE) {}

	void writeHex(u32int number) {
		doCall(VT_IFACE_WRITEHEX, number);
	}

	void put(WChar c) {
		doCall(VT_IFACE_PUT, c);
	}
};
