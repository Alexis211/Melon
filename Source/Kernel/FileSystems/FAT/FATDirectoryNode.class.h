#ifndef DEF_FATDIRECTORYNODE_CLASS_H
#define DEF_FATDIRECTORYNODE_CLASS_H

#include <VFS/DirectoryNode.class.h>

class FATDirectoryNode : public DirectoryNode {
	friend class FATFS;
	FATDirectoryNode(String name, FileSystem* fs, FSNode* parent, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) :
			DirectoryNode(name, fs, parent, permissions, uid, gid) {}

	u32int m_firstCluster;
	u32int m_firstDirEntryID;	//id of the first directory entry corresponding to this file in parent directory
};

#endif
