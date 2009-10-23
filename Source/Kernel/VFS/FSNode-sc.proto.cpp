#include "FSNode.proto.h"
#include <VFS/VFS.ns.h>
#include <SyscallManager/Res.ns.h>
#include <UserManager/Usr.ns.h>
#include <TaskManager/Task.ns.h>

call_t FSNode::m_callTable[] = {
	CALL0(FNIF_GETNAME, &FSNode::getNameSC),
	CALL0(FNIF_TYPE, &FSNode::typeSC),
	CALL0(FNIF_GETPARENT, &FSNode::getParentSC),
	CALL0(FNIF_GETLENGTH, &FSNode::getLengthSC),
	CALL0(FNIF_GETUID, &FSNode::getUid),
	CALL0(FNIF_GETGID, &FSNode::getGid),
	CALL0(FNIF_GETPERM, &FSNode::getPermissions),
	CALL0(FNIF_GETPATH, &FSNode::getPathSC),
	CALL0(FNIF_SETCWD, &FSNode::setCwdSC),
	CALL0(FNIF_REMOVE, &FSNode::removeSC),
	CALL0(0, 0)
};

u32int FSNode::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == FNIF_SGETRFN) return VFS::getRootNode()->resId();
	if (wat == FNIF_SGETCWD) return Task::currProcess()->getCwd()->resId();
	if (wat == FNIF_SFIND) {
		String* path = (String*)a;
		FSNode* n;
		if (b == 0) {
			n = VFS::find(*path);
		} else {
			n = VFS::find(*path, Res::get<DirectoryNode>(b, FNIF_OBJTYPE));
		}
		if (n != 0) return n->resId();
	}
	if (wat == FNIF_SMKDIR) {
		String* path = (String*)a;
		FSNode* n;
		if (b == 0) {
			n = VFS::mkdir(*path);
		} else {
			n = VFS::mkdir(*path, Res::get<DirectoryNode>(b, FNIF_OBJTYPE));
		}
		if (n != 0) return n->resId();
	}
	return (u32int) - 1;
}

u32int FSNode::getNameSC() {
	return getName().serialize();
}

u32int FSNode::typeSC() {
	return type();
}

u32int FSNode::getLengthSC() {
	u64int* a = (u64int*)Mem::mkXchgSpace(sizeof(u64int));
	*a = getLength();
	return (u32int)a;
}

u32int FSNode::getParentSC() {
	if (m_parent != 0) return m_parent->resId();
	return (u32int) - 1;
}

u32int FSNode::getPathSC() {
	return VFS::path(this).serialize();
}

u32int FSNode::setCwdSC() {
	if (type() == NT_DIRECTORY) {
		Task::currProcess()->setCwd((DirectoryNode*)this);
	}
	return 0;
}

u32int FSNode::removeSC() {
	if (!writable()) return 0;
	return (VFS::remove(this) ? 1 : 0);
}

bool FSNode::readable(User* user) {
	if (ISROOT) return true;
	if (user == 0) user = Usr::user();
	if (user->getUid() == m_uid)
		return ((m_permissions >> 6) & 4) != 0;
	if (user->isInGroup(m_gid))
		return ((m_permissions >> 3) & 4) != 0;
	return (m_permissions & 4) != 0;
}

bool FSNode::writable(User* user) {
	if (ISROOT) return true;
	if (user == 0) user = Usr::user();
	if (user->getUid() == m_uid)
		return ((m_permissions >> 6) & 2) != 0;
	if (user->isInGroup(m_gid))
		return ((m_permissions >> 3) & 2) != 0;
	return (m_permissions & 2) != 0;
}

bool FSNode::runnable(User* user) {
	if (ISROOT) return true;
	if (user == 0) user = Usr::user();
	if (user->getUid() == m_uid)
		return ((m_permissions >> 6) & 1) != 0;
	if (user->isInGroup(m_gid))
		return ((m_permissions >> 3) & 1) != 0;
	return (m_permissions & 1) != 0;
}

bool FSNode::accessible() {
	return readable();
}
