#include "FSNode.proto.h"
#include <VFS/VFS.ns.h>

call_t FSNode::m_callTable[] = {
	CALL0(FNIF_GETNAME, &FSNode::getNameSC),
	CALL0(FNIF_TYPE, &FSNode::typeSC),
	CALL0(FNIF_GETPARENT, &FSNode::getParentSC),
	CALL0(FNIF_GETLENGTH, &FSNode::getLengthSC),
	CALL0(0, 0)
};

u32int FSNode::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == FNIF_SGETRFN) return VFS::getRootNode()->resId();
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
