#ifndef DEF_FILE_CLASS_H
#define DEF_FILE_CLASS_H

#include <File.iface.h>
#include <Binding/FSNode.class.h>
#include <Syscall/RessourceCaller.class.h>
#include <Binding/VirtualTerminal.class.h>
#include <ByteArray.class.h>

class File : public RessourceCaller {
	public:
	File(String name, u8int mode, FSNode start = FSNode(0)) :
   		RessourceCaller(sCall(FLIF_OBJTYPE, FLIF_SOPEN, (u32int)&name, mode, start.resId()), FLIF_OBJTYPE) {
	}
	File(u32int id) : RessourceCaller(id, FLIF_OBJTYPE) {}

	void close() {
		doCall(FLIF_CLOSE);
	}
	bool validOpened() {
		return (doCall(FLIF_VALID) != 0);
	}
	u32int read(u32int max_length, u8int *ptr) {
		return (doCall(FLIF_READ, max_length, (u32int)ptr));
	}
	bool write(u32int length, u8int* ptr) {
		return (doCall(FLIF_WRITE, length, (u32int)ptr) != 0);
	}
	u32int read(ByteArray &data) {
		if (!valid()) {
			data.clear();
			return 0;
		}
		u32int l = read(data.size(), (u8int*)data);
		if (l != data.size()) data.resize(l);
		return l;
	}
	bool write(ByteArray &data) {
		if (!valid()) return false;
		return write(data.size(), (u8int*)data);
	}

	template <typename T> bool read(T* elem) {
		return (read(sizeof(T), (u8int*)elem) == sizeof(T));
	}
	template <typename T> bool write(T* elem) {
		return write(sizeof(T), (u8int*)elem);
	}

	bool seek(u64int count, u8int mode) {
		union {
			u64int x;
			u32int a[2];
		} wat = {count};
		return (doCall(FLIF_SEEK, wat.a[0], wat.a[1], mode) != 0);
	}
	u64int position() {
		return *((u64int*)doCall(FLIF_POSITION));
	}
	u64int length() {
		return *((u64int*)doCall(FLIF_LENGTH));
	}
	bool eof() {
		return (doCall(FLIF_EOF) != 0);
	}
};

#endif
