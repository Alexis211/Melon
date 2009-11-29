#ifndef DEF_FSNODE_PROTO_H
#define DEF_FSNODE_PROTO_H

#include <common.h>
#include <String.class.h>

class FSNode;
#include <VFS/FileSystem.proto.h>
#include <SyscallManager/Ressource.class.h>

#include <FSNode.iface.h>
#include <UserManager/User.class.h>

class FSNode : public Ressource {
	protected:
	String m_name;
	u64int m_length;
	u32int m_permissions, m_uid, m_gid;
	FileSystem *m_fs;
	FSNode *m_parent;

	//Syscall related
	static call_t m_callTable[];
	u32int getNameSC();
	u32int getLengthSC();
	u32int typeSC();
	u32int getParentSC();
	u32int getPathSC();
	u32int setCwdSC();
	u32int removeSC();
	bool accessible();
	
	public:
	static u32int scall(u8int, u32int, u32int, u32int, u32int);

	FSNode(String name, FileSystem* fs, FSNode* parent, u64int length = 0, u32int permissions = 0777, 
			u32int uid = 0, u32int gid = 0) :
		Ressource(FNIF_OBJTYPE, m_callTable), m_name(name), m_length(length), m_permissions(permissions),
		m_uid(uid), m_gid(gid), m_fs(fs), m_parent(parent) {}
	virtual ~FSNode() {}

	virtual u8int type() = 0;
	virtual bool removable() = 0;	//True for files, false for non-empty directories, true otherwise
	virtual bool used() { return false; }	//True if file is read/written from/to
	
	const String& getName() { return m_name; }
	virtual u64int getLength() { return m_length; }
	u32int getPermissions() { return m_permissions; }
	u32int getUid() { return m_uid; }
	u32int getGid() { return m_gid; }
	FileSystem *getFS() { return m_fs; }
	virtual FSNode* getParent() { return m_parent; }

	//Helper functions
	bool readable(User* user = 0);
	bool writable(User* user = 0);
	bool runnable(User* user = 0);

	public:
	bool setName(String name) { 
		bool b = m_fs->setName(this, name);
		if (b) m_name = name;
		return b;
   	}
	bool setPermissions(u32int permissions) {
		bool b = m_fs->setPermissions(this, permissions);
		if (b) m_permissions = permissions;
		return b;
   	}
	bool setUid(u32int uid) {
		bool b = m_fs->setUid(this, uid);
		if (b) m_uid = uid;
		return b;
   	}
	bool setGid(u32int gid) {
		bool b = m_fs->setGid(this, gid);
		if (b) m_gid = gid;
		return b;
	}
	bool setParent(FSNode* parent) {
		bool b = m_fs->setParent(this, parent);	//FSSetParent is only expected to move files/directories in the same filesystem
		if (b) m_parent = parent;
		return b;
   	}
};

#endif
