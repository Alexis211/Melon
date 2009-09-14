#include "RamFS.class.h"
#include <VFS/DirectoryNode.class.h>
#include "RamFileNode.class.h"

RamFS::RamFS(u32int maxSize) {
	m_maxSize = maxSize;
	m_usedSize = 0;
	m_isWritable = true;
	m_rootNode = new DirectoryNode("/", this, NULL);	
}

RamFS::RamFS(u8int *ptr, u32int maxSize, bool writable) {
	m_maxSize = maxSize;
	m_usedSize = 0;
	m_isWritable = true;
	m_rootNode = new DirectoryNode("/", this, NULL);

	union {
		u8int* c;
		initrd_header* i;
		initrd_file_header* f;
	} curr;
	curr.c = ptr;

	if (curr.i->magic != INITRD_MAGIC) return;
	u32int files = curr.i->files;
	curr.i++;	//Increment pointer of size of initrd header
	DEBUG_HEX(files); DEBUG(" is initrd file count.");
	for (u32int i = 0; i < files; i++) {
		initrd_file_header h = *(curr.f);
		curr.f++;	//Increment pointer of size of file header
		if (h.name_length != 0 or h.file_length != 0) {
			String name((const char*)(curr.c));
			curr.c += h.name_length + 1;	//Increment pointer of length of name

			//Find out a vector conaining parent directories, and set name to the effective file name
			if (name[0] == WChar("/")) name = name.substr(1, name.size() - 1);

			//Find node for parent directory
			String mname = "";
			DirectoryNode* parent = m_rootNode;
			for (u32int i = 0; i < name.size(); i++) {
				if (name[i] == WChar("/")) {
					FSNode* n = parent->getChild(mname);
					if (n == NULL) break;
					if (n->type() != NT_DIRECTORY) break;
					parent = (DirectoryNode*)n;
					mname.clear();
				} else {
					mname += name[i];
				}
			}
			name = mname;

			//Add new node
			if (h.file_length == 0) {
				parent->createDirectory(name);
			} else {
				FileNode* file = parent->createFile(name);
				file->write(0, h.file_length, curr.c);
				curr.c += h.file_length;
			}
		}	
	}
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

bool RamFS::remove(DirectoryNode* parent, FSNode* node) { 
	if (node->type() == NT_FILE) {
		u8int *d = ((RamFileNode*)node)->m_data;
		if (d != 0) Mem::kfree(d);
	}
	return true;
}