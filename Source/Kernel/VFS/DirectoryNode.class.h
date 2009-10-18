#ifndef DEF_DIRECTORYNODE_CLASS_H
#define DEF_DIRECTORYNODE_CLASS_H

#include <VFS/FileNode.class.h>
#include <Vector.class.h>

class DirectoryNode : public FSNode {
	protected:
	Vector<FSNode*> m_children;
	bool m_contentLoaded;

	public:
	DirectoryNode(String name, FileSystem* fs, FSNode* parent, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) : 
			FSNode(name, fs, parent, 0, permissions, uid, gid), m_children(), m_contentLoaded(false) {}
	virtual ~DirectoryNode() {
		if (m_contentLoaded) {
			for (u32int i = 0; i < m_children.size(); i++) {
				delete m_children[i];
			}
		}
	}

	Vector<FSNode*> &getChildren() { return m_children; }	//MUST BE USED ONLY BY FILESYSTEMS

	u8int type() { return NT_DIRECTORY; }
	bool removable();
	bool unmountable();

	bool loadContent();
	FSNode* getChild(u32int index);
	FSNode* getChild(const String& name);
	FileNode* createFile(const String& name);
	DirectoryNode* createDirectory(const String& name);
	bool remove(FSNode* child);	//Removes a child node from this directory
};

#endif
