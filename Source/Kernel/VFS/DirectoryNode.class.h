#ifndef DEF_DIRECTORYNODE_CLASS_H
#define DEF_DIRECTORYNODE_CLASS_H

#include <VFS/FileNode.class.h>
#include <Library/Vector.class.h>

class DirectoryNode : public FSNode {
	protected:
	Vector<FSNode*> m_children;
	bool m_contentLoaded;

	public:
	DirectoryNode(String name, FileSystem* fs, FSNode* parent, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) : 
			FSNode(name, fs, parent, 0, permissions, uid, gid), m_children(), m_contentLoaded(false) {}

	Vector<FSNode*> &getChildren() { return m_children; }	//MUST BE USED ONLY BY FILESYSTEMS

	u8int type() { return NT_DIRECTORY; }

	bool loadContent() {
		if (m_contentLoaded) return true;
		bool b = m_fs->loadContents(this);
		if (!b) return false;
		m_length = m_children.size();
		m_contentLoaded = true;
		return b;
	}

	FSNode* getChild(u32int index) {
		if (!m_contentLoaded)
			if (!loadContent())
				return NULL;
		if (index >= m_children.size()) return NULL;
		return m_children[index];
	}

	FSNode* getChild(String name) {
		if (!m_contentLoaded)
			if (!loadContent())
				return NULL;
		for (u32int i = 0; i < m_children.size(); i++) {
			if (name == m_children[i]->getName())
				return m_children[i];
		}
		return NULL;
	}
	FileNode* createFile(String name) {
		FileNode* n = m_fs->createFile(this, name);
		m_length = m_children.size();
		return n;
	}
	DirectoryNode* createDirectory(String name) {
		DirectoryNode* n = m_fs->createDirectory(this, name);
		m_length = m_children.size();
		return n;
	}
};

#endif
