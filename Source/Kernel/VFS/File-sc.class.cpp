#include "File.class.h"
#include <VFS/VFS.ns.h>
#include <TaskManager/Task.ns.h>
#include <FSNode.iface.h>
#include <SyscallManager/Res.ns.h>

u32int File::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == FLIF_SOPEN) {
		String* name = (String*)a;
		FSNode* start = Res::get<FSNode>(c, FNIF_OBJTYPE);
		File* f = new File();
		if (!f->open(*name, b, start, true)) {
			delete f;
		} else {
			return f->resId();
		}
	}
	return (u32int) - 1;
}

u32int File::closeSC() {
	if (!valid()) return 1;
	close();
	return 0;
}

u32int File::validSC() {
	return (valid() ? 1 : 0);
}

u32int File::readSC(u32int max_length, u32int ptr) {
	if (!m_file->readable()) return 0;
	return read(max_length, (u8int*)ptr);
}

u32int File::writeSC(u32int length, u32int ptr) {
	if (!m_file->writable()) return 0;
	return (write(length, (u8int*)ptr) ? 1 : 0);
}

u32int File::seekSC(u32int counta, u32int countb, u32int mode) {
	union {
		u32int a[2];
		u64int x;
	} wat = {{counta, countb}};
	return (seek(wat.x, mode) ? 1 : 0);
}

u32int File::positionSC() {
	u64int* w = (u64int*)Mem::mkXchgSpace(sizeof(u64int));
	*w = position();
	return (u32int)w;
}

u32int File::lengthSC() {
	u64int* w = (u64int*)Mem::mkXchgSpace(sizeof(u64int));
	*w = length();
	return (u32int)w;
}

u32int File::eofSC() {
	return (eof() ? 1 : 0);
}

bool File::accessible() {
	if (ISROOT or Usr::uid() == m_process->getUid()) return true;
	return false;
}
