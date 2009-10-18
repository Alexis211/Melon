#include <Syscall/RessourceCaller.class.h>

#include <VirtualTerminal.iface.h>

#include <String.class.h>
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
	void writeDec(s64int number) {
		doCall(VT_IFACE_WRITEDEC, (number >> 32), number);
	}
	void write(String s) {
		Serialized a = s.serialize();
		doCall(VT_IFACE_WRITE, a);
	}
	String readLine() {
		return String::unserialize(doCall(VT_IFACE_READLINE));
	}

	void put(WChar c) {
		doCall(VT_IFACE_PUT, c);
	}
};
