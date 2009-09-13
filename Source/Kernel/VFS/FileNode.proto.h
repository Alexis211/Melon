#ifndef DEF_FILENODE_PROTO_H
#define DEF_FILENODE_PROTO_H

#include <VFS/FSNode.proto.h>

class FileNode : public virtual FSNode {
	protected:
	DirectoryNode() {}

	virtual bool FSRead(u64int position, u32int max_length, u8int *data) = 0;
	virtual bool FSWrite(u64int position, u32int length, u8int *data) = 0;

	public:
	bool read(u64int position, u32int max_length, u8int *data) {
		return FSRead(position, max_length, data);
	}

	bool write(u64int position, u32int length, u8int *data) {
		return FSWrite(position, length, data);
	}
	
};

#endif
