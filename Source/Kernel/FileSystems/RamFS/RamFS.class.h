#ifndef DEF_RAMFS_CLASS_H
#define DEF_RAMFS_CLASS_H

#include <VFS/FileSystem.proto.h>

struct initrd_header {
	unsigned int magic;
	unsigned int files;
};

struct initrd_file_header {
	unsigned int name_length;
	unsigned int file_length;
};

#define INITRD_MAGIC 0x1337BEEF

class RamFS : public FileSystem {
	private:
	u32int m_maxSize;
	u32int m_usedSize;

	public:
	RamFS(u32int maxSize);	//Creates an empty RAM file system
	RamFS(u8int* ptr, u32int maxSize, bool writable = true);	//Creates a RAM file system from data loaded in memory. format to be defined

	bool setName(FSNode* node, String name);
	bool setPermissions(FSNode* node, u32int permissions);
	bool setUid(FSNode* node, u32int uid);
	bool setGid(FSNode* node, u32int gid);
	bool setParent(FSNode* node, FSNode* parent);

	u32int read(FileNode* file, u64int position, u32int max_length, u8int *data);
	bool write(FileNode* file, u64int position, u32int length, u8int *data);
	bool truncate(FileNode* file);

	bool loadContents(DirectoryNode* dir);
	FileNode* createFile(DirectoryNode* parent, String name);
	DirectoryNode* createDirectory(DirectoryNode* parent, String name);
	bool remove(DirectoryNode* parent, FSNode* node);
};

#endif