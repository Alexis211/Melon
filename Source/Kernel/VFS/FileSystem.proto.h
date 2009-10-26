#ifndef DEF_FILESYSTEM_PROTO_H
#define DEF_FILESYSTEM_PROTO_H

#include <VFS/Partition.class.h>
class FSNode;
class FileNode;
class DirectoryNode;

//This abstract class describes a filesystem
class FileSystem {
	protected:
	virtual ~FileSystem() {}
	bool m_isWritable;	//false = read only
	DirectoryNode* m_rootNode;

	public:
	bool unmount();

	bool isWritable() { return m_isWritable; }
	DirectoryNode* getRootNode() { return m_rootNode; }

	//Must be implemented by the filesystem
	virtual bool setName(FSNode* node, String name) = 0;
	virtual bool setPermissions(FSNode* node, u32int permissions) = 0;
	virtual bool setUid(FSNode* node, u32int uid) = 0;
	virtual bool setGid(FSNode* node, u32int gid) = 0;
	virtual bool setParent(FSNode* node, FSNode* parent) = 0;

	virtual u32int read(FileNode* file, u64int position, u32int max_length, u8int *data) = 0;
	virtual bool write(FileNode* file, u64int position, u32int length, u8int *data) = 0;
	virtual bool truncate(FileNode* file) = 0;

	virtual bool loadContents(DirectoryNode* dir) = 0;
	virtual FileNode* createFile(DirectoryNode* parent, String name) = 0;
	virtual DirectoryNode* createDirectory(DirectoryNode* parent, String name) = 0;
	virtual bool remove(DirectoryNode* parent, FSNode* node) = 0;
};

#endif
