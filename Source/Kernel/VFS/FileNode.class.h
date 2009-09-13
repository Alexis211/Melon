#ifndef DEF_FILENODE_CLASS_H
#define DEF_FILENODE_CLASS_H

#include <VFS/FSNode.proto.h>

class FileNode : public FSNode {
	protected:
	FileNode(String name, FileSystem* fs, FSNode* parent, u32int length = 0, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0): FSNode(name, fs, parent, length, permissions, uid, gid) {}

	public:
	virtual ~FileNode() {}

	u8int type() { return NT_FILE; }
	bool removable() { return true; }

	u32int read(u64int position, u32int max_length, u8int *data) {
		return m_fs->read(this, position, max_length, data);
	}

	bool write(u64int position, u32int length, u8int *data) {
		return m_fs->write(this, position, length, data);
	}

	bool truncate() {
		return m_fs->truncate(this);
	}
	
};

#endif
