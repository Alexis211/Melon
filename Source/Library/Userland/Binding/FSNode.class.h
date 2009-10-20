#include <Syscall/RessourceCaller.class.h>
#include <FSNode.iface.h>

class FSNode : public RessourceCaller {
	public:
	static FSNode getRoot() {
		return FSNode(RessourceCaller::sCall(FNIF_OBJTYPE, FNIF_SGETRFN));
	}
	FSNode(u32int id) : RessourceCaller(id, FNIF_OBJTYPE) {}

	String getName() {
		return String::unserialize(doCall(FNIF_GETNAME));
	}
	u8int type() {
		return doCall(FNIF_TYPE);
	}
	FSNode getParent() {
		return FSNode(doCall(FNIF_GETPARENT));
	}
	u64int getLength() {
		return *((u64int*)doCall(FNIF_GETLENGTH));
	}
};
