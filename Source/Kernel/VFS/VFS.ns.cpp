#include "VFS.ns.h"
#include <VFS/FileNode.class.h>
#include <Vector.class.h>
#include <VTManager/VirtualTerminal.proto.h>

#include <VFS/Part.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>
#include <FileSystems/FAT/FATFS.class.h>

struct local_fs_t {
	const char* name;
	mount_callback_t cb;
} fileSystems[] = {
	{"fat", FATFS::mount},
	{0, 0}
};

FileSystem::~FileSystem() { delete m_rootNode; }

namespace VFS {

DirectoryNode *rootNode = 0;
Vector<FileSystem*> filesystems;

DirectoryNode* getRootNode() {
	return rootNode;
}

void registerFilesystem(FileSystem* fs) {
	unregisterFilesystem(fs);
	filesystems.push(fs);
	if (rootNode == 0) rootNode = fs->getRootNode();
}

void unregisterFilesystem(FileSystem* fs) {
	for (u32int i = 0; i < filesystems.size(); i++) {
		if (filesystems[i] == fs) {
			filesystems[i] = filesystems.back();
			filesystems.pop();
			break;
		}
	}
}

bool unmount(FileSystem* fs) {
	if (!fs->getRootNode()->unmountable()) return false;
	if (fs->getRootNode() == rootNode) return false;
	if (!fs->unmount()) return false;
	delete fs;	//Will automatically delete the root node (destructor is in this file);
	return true;
}

bool mount(String str, VirtualTerminal* vt, multiboot_info_t *mbd) {
	Vector<String> fs = str.split(":");
	DirectoryNode* root;
	if (fs[0] == "/") {
		root = NULL;
	} else {
		FSNode* n = VFS::find(fs[0]);
		if (n == NULL) {
			*vt << "Mountpoint does not exist : " << fs[0] << "\n";
			return false;
		}
		if (n->type() != NT_DIRECTORY) {
			*vt << "Mountpoint is not a directory : " << fs[0] << "\n";
			return false;
		}
		root = (DirectoryNode*)n;
	}
	if (fs[1] == "ramfs") {
		if (fs.size() > 2) {
			if (mbd != 0) {
				module_t *mods = (module_t*)mbd->mods_addr;
				if (fs[2].toInt() >= mbd->mods_count) {
					*vt << "Invalid module number for filesystem to mount on " << fs[0] << "\n";
					return false;
				}
				RamFS::mount((u8int*)mods[fs[2].toInt()].mod_start, 1024 * 1024, root);
				return true;
			} else {
				*vt << "Cannot mount kernel modules outside of kernel command line.\n";
				return false;
			}
		} else {
			RamFS::mount(1024 * 1024, root);
			return true;
		}
	} else {
		if (fs.size() < 4) {
			*vt << "Syntax: <mountpoint>:[<dev_class>]:<dev_id>:<part_id>[:<fs_type>[:[ro|rw]]]\n";
			return false;
		}
		if (fs.size() < 5) fs.push("");
		if (fs.size() < 6) fs.push("ro");	//By default, mount file systems read-only
		BlockDevice* d = Part::dev(fs[1], fs[2].toInt());
		Partition* p = Part::part(d, fs[3].toInt());
		for (u32int i = 0; fileSystems[i].cb != 0; i++) {
			if (fs[4] == fileSystems[i].name or fs[4] == "") {
				if (fileSystems[i].cb(p, root, (fs[5] == "rw")) != NULL) {
					return true;
				} else if (fs[4] != "") {
					*vt << "Could not mount filesystem on " << fs[0] << "\n";
					if (root == NULL) PANIC("Error while mounting root filesystem.");
					return false;
				}
			}
		}
		*vt << "Unknown filesystem type for filesystem to mount on " << fs[0] << "\n";
		if (root == NULL) PANIC("Unknown filesystem type for root file system.");
		return false;
	}
}

FSNode* find(const String& path, FSNode* start) {
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");

	FSNode* node = start;
	if (p[0].empty()) node = rootNode;	//Empty first element means string starts with "/"
	for (u32int i = 0; i < p.size(); i++) {
		if (p[i] == "..") {
			node = node->getParent();
		} else if (!p[i].empty() and p[i] != ".") {
			if (node->type() == NT_DIRECTORY) {
				node = ((DirectoryNode*)node)->getChild(p[i]);
			} else {
				node = NULL;
			}
		}
		if (node == NULL) return node;
	}

	return node;
}

FSNode* createFile(const String& path, FSNode* start, bool vrfyperm) {
	if (find(path, start) != NULL) return NULL;	//Something already has that name.
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");
	String name = p.back();
	p.pop();

	FSNode* node = start;
	if (!path.empty()) {
		if (p[0].empty()) node = rootNode;
		for (u32int i = 0; i < p.size(); i++) {
			if (p[i] == "..") {
				node = node->getParent();
			} else if (!p[i].empty() and p[i] != ".") {
				if (node->type() == NT_DIRECTORY) {
					node = ((DirectoryNode*)node)->getChild(p[i]);
				} else {
					node = NULL;
				}
			}
			if (node == NULL) return node;
		}
	}

	if (node->type() == NT_DIRECTORY) {
		if ((vrfyperm && node->writable()) or !vrfyperm) return ((DirectoryNode*)node)->createFile(name);
	}
	return NULL;
}

FSNode* createDirectory(const String& path, FSNode* start, bool vrfyperm) {
	if (find(path, start) != NULL) return NULL;	//Something already has that name.
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");
	String name = p.back();
	p.pop();

	FSNode* node = start;
	if (!path.empty()) {
		if (p[0].empty()) node = rootNode;
		for (u32int i = 0; i < p.size(); i++) {
			if (p[i] == "..") {
				node = node->getParent();
			} else if (!p[i].empty() and p[i] != ".") {
				if (node->type() == NT_DIRECTORY) {
					node = ((DirectoryNode*)node)->getChild(p[i]);
				} else {
					node = NULL;
				}
			}
			if (node == NULL) return node;
		}
	}

	if (node->type() == NT_DIRECTORY) {
		if ((vrfyperm && node->writable()) or !vrfyperm) return ((DirectoryNode*)node)->createDirectory(name);
	}
	return NULL;
}

bool remove(FSNode* node) {
	FSNode* parent = node->getParent();
	if (parent == NULL) return false;

	if (parent->type() == NT_DIRECTORY) {
		return ((DirectoryNode*)parent)->remove(node);
	} else {
		return false;	//Something wierd happenned
	}
}

bool remove(const String& path, FSNode* start) {
	FSNode* node = find(path, start);
	if (node == NULL) return false;
	return remove(node);
}

String path(FSNode* node) {
	String path;

	while (node != NULL) {
		String t = "/";
		t += node->getName();
		if (t != "//") {
			t += path;
			path = t;
		}
		node = node->getParent();
	}

	if (path.empty()) path = "/";

	return path;
}

}
