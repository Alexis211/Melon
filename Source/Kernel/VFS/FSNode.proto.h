#ifndef DEF_FSNODE_PROTO_H
#define DEF_FSNODE_PROTO_H

#include <Core/common.wtf.h>
#include <Library/String.class.h>

class FileSystem;

enum {
	NT_FILE = 1,
	NT_DIRECTORY = 2,
	NT_SYMLINK = 3,
	NT_MOUNTPOINT = 4
};

class FSNode {
	protected:
	String m_name;
	u32int m_length;
	u32int m_permissions, m_uid, m_gid;
	FileSystem *m_fs;
	FSNode *m_parent;
	u32int m_inode;

	FSNode(String name, FileSystem* fs, FSNode* parent, u32int inode, u32int length = 0, u32int permissions = 0777, 
			u32int uid = 0, u32int gid = 0) :
		m_name(name), m_fs(fs), m_parent(parent), m_inode(inode), m_length(length), m_premissions(permissions),
		m_uid(uid), m_gid(gid) {}
	
	public:

	virtual u8int type() = 0;
	
	const String& getName() { return m_name; }
	u32int getLength() { return m_length; }
	u32int getPermissions() { return m_permissions; }
	u32int getUid() { return m_uid; }
	u32int getGid() { return m_gid; }
	FileSystem *getFS() { return m_fs; }
	FSNode* getParent() { return m_parent; }
	u32int getInode() { return m_inode; }

	protected:
	//Must be implemented by *FSNode
	virtual bool FSSetName(String name) = 0;
	virtual bool FSTruncate() = 0;
	virtual bool FSSetPermissions(u32int permissions) = 0;
	virtual bool FSSetUid(u32int uid) = 0;
	virtual bool FSSetGid(u32int gid) = 0;
	virtual bool FSSetParent(FSNode* parent) = 0;

	public:
	bool setName(String name) { 
		bool b = FSSetName(name);
		if (b) m_name = name;
		return b;
   	}
	bool truncate() {
		bool b = FSTruncate();
		if (b) m_length = 0;
		return b;
	}
	bool setPermissions(u32int permissions) {
		bool b = FSSetPermissions(permissions);
		if (b) m_permissions = permissions;
		return b;
   	}
	bool setUid(u32int uid) {
		bool b = FSSetUid(uid);
		if (b) m_uid = uid;
		return b;
   	}
	bool setGid(u32int gid) {
		bool b = FSSetGid(gid);
		if (b) m_gid = gid;
		return b;
	}
	bool setParent(FSNode* parent) {
		bool b = FSSetParent(parent);	//FSSetParent is only expected to move files/directories in the same filesystem
		if (b) m_parent = parent;
		return b;
   	}
	void setInode(u32int inode) { m_inode = inode; }
};

#endif
