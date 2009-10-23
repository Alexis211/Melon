#include <Syscall/RessourceCaller.class.h>
#include <FSNode.iface.h>

class FSNode : public RessourceCaller {
	public:
	static FSNode getRoot() {
		return FSNode(sCall(FNIF_OBJTYPE, FNIF_SGETRFN));
	}
	static FSNode getCwd() {
		return FSNode(sCall(FNIF_OBJTYPE, FNIF_SGETCWD));
	}
	static FSNode find(String path) {	//Finds a node starting from root node
		return FSNode(sCall(FNIF_OBJTYPE, FNIF_SFIND, (u32int)&path, 0));
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
	u32int getUid() {
		return doCall(FNIF_GETUID);
	}
	u32int getGid() {
		return doCall(FNIF_GETGID);
	}
	u32int getPerm() {
		return doCall(FNIF_GETPERM);
	}
	String path() {
		return String::unserialize(doCall(FNIF_GETPATH));
	}
	void setCwd() {
		doCall(FNIF_SETCWD);
	}
	FSNode getChild(u32int idx) {
		return FSNode(doCall(FNIF_GETIDXCHILD, idx));
	}
	FSNode getChild(String name) {
		return FSNode(doCall(FNIF_GETNAMECHILD, (u32int)&name));
	}
	FSNode findFrom(String path) {		//Search a filesystem node starting from here
		return FSNode(sCall(FNIF_OBJTYPE, FNIF_SFIND, (u32int)&path, resId()));
	}
};
