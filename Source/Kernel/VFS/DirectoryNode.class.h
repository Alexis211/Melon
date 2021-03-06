#ifndef DEF_DIRECTORYNODE_CLASS_H
#define DEF_DIRECTORYNODE_CLASS_H

#include <VFS/FileNode.class.h>
#include <Vector.class.h>

class DirectoryNode : public FSNode {
	protected:
	Vector<FSNode*> m_children;
	bool m_contentLoaded;
	DirectoryNode* m_mounts;	//Root node of the filesystem mounted here, null if none

	//Syscalls
	static call_t m_callTable[];
	u32int getIdxChildSC(u32int index);
	u32int getNameChildSC(u32int name);

	public:
	DirectoryNode(String name, FileSystem* fs, FSNode* parent, u32int permissions = 0777,
			u32int uid = 0, u32int gid = 0) : 
			FSNode(name, fs, parent, 0, permissions, uid, gid), m_children(), m_contentLoaded(false) {
		addCallTable(m_callTable);
		m_mounts = 0;
	}
	virtual ~DirectoryNode();

	Vector<FSNode*> &getChildren() { return m_children; }	//MUST BE USED ONLY BY FILESYSTEMS

	u8int type() { return NT_DIRECTORY; }
	const String& getName();
	u64int getLength();
	FSNode* getParent();
	bool removable();
	bool unmountable();
	bool mountpointable();
	void mount(DirectoryNode* childRoot);
	void unmount();

	bool loadContent();
	FSNode* getChild(u32int index);
	FSNode* getChild(const String& name);
	FileNode* createFile(const String& name);
	DirectoryNode* createDirectory(const String& name);
	bool remove(FSNode* child);	//Removes a child node from this directory
};

#endif
