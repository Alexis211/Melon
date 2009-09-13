#include "RamFS.class.h"
#include <VFS/DirectoryNode.class.h>
#include "RamFileNode.class.h"

RamFS::RamFS(u32int maxSize) {
	m_maxSize = maxSize;
	m_usedSize = 0;
	m_isWritable = true;
	m_rootNode = new DirectoryNode("/", this, NULL);	
}

bool RamFS::setName(FSNode* node, String name) { return true; }
bool RamFS::setPermissions(FSNode* node, u32int permissions) { return true; }
bool RamFS::setUid(FSNode* node, u32int uid) { return true; }
bool RamFS::setGid(FSNode* node, u32int gid) { return true; }
bool RamFS::setParent(FSNode* node, FSNode* parent) {
	if (parent->getFS() == this) return true;
	return false;
}

u32int RamFS::read(FileNode* file, u64int position, u32int max_length, u8int *data) {
	RamFileNode *node = (RamFileNode*) file;
	if (file->getLength() <= position) return 0;
	u32int length = file->getLength() - position;
	if (length > max_length) length = max_length;
	memcpy(data, node->m_data + position, length);
	return length;
}

bool RamFS::write(FileNode* file, u64int position, u32int length, u8int *data) {
	if (!m_isWritable) return false;
	RamFileNode *node = (RamFileNode*) file;

	u32int end = position + length;
	if (end > node->getLength()) {
		if (m_usedSize - node->getLength() + end > m_maxSize) return false;
		m_usedSize -= node->getLength();
		m_usedSize += end;

		u8int* data = (u8int*)Mem::kalloc(end);
		if (data == 0) return false;	//Invalid pointer
		if (node->m_data != 0) {
			memcpy(data, node->m_data, node->getLength());
			Mem::kfree(node->m_data);
		}
		node->m_data = data;
		node->setLength(end);
	}
	memcpy(node->m_data + position, data, length);
	return true;
}

bool RamFS::truncate(FileNode* file) {
	if (!m_isWritable) return false;
	RamFileNode *node = (RamFileNode*) file;

	Mem::kfree(node->m_data);
	node->setLength(0);
	node->m_data = 0;

	return true;
}

bool RamFS::loadContents(DirectoryNode* dir) { return true; }	//Nothing to do.

FileNode* RamFS::createFile(DirectoryNode* parent, String name) {
	if (!m_isWritable) return NULL;
	if (parent->getFS() != this) return NULL;

	RamFileNode* n = new RamFileNode(name, this, parent);
	parent->loadContent();
	parent->getChildren().push(n);

	return n;
}

DirectoryNode* RamFS::createDirectory(DirectoryNode* parent, String name) {
	if (!m_isWritable) return NULL;
	if (parent->getFS() != this) return NULL;

	DirectoryNode* d = new DirectoryNode(name, this, parent);
	parent->loadContent();
	parent->getChildren().push(d);

	return d;
}
