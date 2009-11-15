#ifndef DEF_RAMFILENODE_CLASS_H
#define DEF_RAMFILENODE_CLASS_H

#include <VFS/FileNode.class.h>

class RamFileNode : public FileNode {
	friend class RamFS;

	private:
	RamFileNode (String name, FileSystem* fs, FSNode* parent, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) :
		FileNode(name, fs, parent, 0, permissions, uid, gid), m_data(0) {}

	~RamFileNode() { if (m_data != 0) delete m_data; }

	u8int *m_data;		//We don't take care of allocation/freeing here, RamFS:: does that for us

	void setLength(u32int length) { m_length = length; }
};

#endif
