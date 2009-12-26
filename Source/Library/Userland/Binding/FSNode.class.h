#ifndef DEF_FSNODE_CLASS_H
#define DEF_FSNODE_CLASS_H

#include <Syscall/RessourceCaller.class.h>
#include <String.class.h>
#include <FSNode.iface.h>

class FSNode : public RessourceCaller {
	public:
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
	bool remove() {
		return doCall(FNIF_REMOVE) != 0;
	}
	FSNode getChild(u32int idx) {
		return FSNode(doCall(FNIF_GETIDXCHILD, idx));
	}
	FSNode getChild(String name) {
		return FSNode(doCall(FNIF_GETNAMECHILD, (u32int)&name));
	}
};

namespace FS {

inline FSNode rootNode() {
	return FSNode(RessourceCaller::sCall(FNIF_OBJTYPE, FNIF_SGETRFN));
}

inline FSNode cwdNode() {
	return FSNode(RessourceCaller::sCall(FNIF_OBJTYPE, FNIF_SGETCWD));
}

inline FSNode find(String name, FSNode cwd = FSNode(0)) {
	if (!cwd.valid()) cwd = rootNode();
	return FSNode(RessourceCaller::sCall(FNIF_OBJTYPE, FNIF_SFIND, (u32int)&name, cwd.resId()));
}

inline FSNode mkdir(String name, FSNode cwd = FSNode(0)) {
	if (!cwd.valid()) cwd = rootNode();
	return FSNode(RessourceCaller::sCall(FNIF_OBJTYPE, FNIF_SMKDIR, (u32int)&name, cwd.resId()));
}

inline String dirname(String filename) {
	int lastSlash = 0;
	for (int i = 0; i < filename.size(); i++) {
		if (filename[i] == WChar("/")) {
			lastSlash = i;
		}
	}
	if (lastSlash == 0 and filename[0] == WChar("/")) return "/";
	return filename.substr(0, lastSlash);
}

inline String basename(String filename) {
	int lastSlash = 0;
	for (int i = 0; i < filename.size(); i++) {
		if (filename[i] == WChar("/")) {
			lastSlash = i;
		}
	}
	if (lastSlash == 0 and filename[0] != WChar("/")) return filename;
	return filename.substr(lastSlash + 1);
}

}

#endif
