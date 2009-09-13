#ifndef DEF_VFS_NS_H
#define DEF_VFS_NS_H

#include <VFS/FSNode.proto.h>
#include <VFS/FileSystem.proto.h>

typedef FileSystem* (* mountcallback)(Partition* partition);

namespace VFS {
	void registerMountCallback(mountcallback mcb);
	bool mount(Partition* partition);
	bool setRootNode(FSNode* root);
}

#endif
