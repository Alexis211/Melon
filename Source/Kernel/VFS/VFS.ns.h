#ifndef DEF_VFS_NS_H
#define DEF_VFS_NS_H

#include <VFS/DirectoryNode.class.h>
#include <VFS/FileSystem.proto.h>

typedef FileSystem* (* mountcallback)(Partition* partition);

namespace VFS {
	void registerMountCallback(mountcallback mcb);
	bool mount(Partition* partition, DirectoryNode *mountpoint);
	bool setRootNode(DirectoryNode* root);
	DirectoryNode* getRootNode();

	FSNode* find(const String& path, FSNode* start = 0);
	FSNode* createFile(const String& path, FSNode* start = 0);
	FSNode* createDirectory(const String& path, FSNode* start = 0);
	bool remove(FSNode* node);
	bool remove(const String& path, FSNode* start = 0);	//Returns false for non-empty directories
	String path(FSNode* node);	//Returns complete path for a node
}

#endif
