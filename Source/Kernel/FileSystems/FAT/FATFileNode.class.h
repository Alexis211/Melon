#ifndef DEF_FATFILENODE_CLASS_H
#define DEF_FATFILENODE_CLASS_H

#include <VFS/FileNode.class.h>

class FATFileNode : public FileNode {
	friend class FATFS;
	FATFileNode (String name, FileSystem* fs, FSNode* parent, u32int size, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) :
		FileNode(name, fs, parent, size, permissions, uid, gid){}

	u32int m_firstCluster;
	u32int m_firstDirEntryID;	//id of the first directory entry corresponding to this file in parent directory
};

#endif
