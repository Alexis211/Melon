#ifndef DEF_VFS_NS_H
#define DEF_VFS_NS_H

#include <VFS/DirectoryNode.class.h>
#include <VFS/FileSystem.proto.h>
#include <Vector.class.h>

typedef FileSystem* (* mount_callback_t)(Partition* partition);

namespace VFS {
	extern Vector<FileSystem*> filesystems;

	void registerMountCallback(mount_callback_t mcb);
	bool mount(Partition* partition, DirectoryNode *mountpoint);
	DirectoryNode* getRootNode();

	void registerFilesystem(FileSystem* fs);
	void unregisterFilesystem(FileSystem* fs);
	bool unmount(FileSystem* fs);

	FSNode* find(const String& path, FSNode* start = 0);
	FSNode* createFile(const String& path, FSNode* start = 0, bool vrfyperm = false);
	FSNode* createDirectory(const String& path, FSNode* start = 0, bool vrfyperm = false);
	bool remove(FSNode* node);
	bool remove(const String& path, FSNode* start = 0);	//Returns false for non-empty directories
	String path(FSNode* node);	//Returns complete path for a node
}

#endif
