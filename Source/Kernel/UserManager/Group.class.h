#ifndef DEF_GROUP_CLASS_H
#define DEF_GROUP_CLASS_H

#include <UserManager/Usr.ns.h>

class Group {
	friend void Usr::load();

	private:
	Group(String name, u32int gid) : m_name(name), m_gid(gid) {}

	String m_name;
	u32int m_gid;

	public:
	void setName(String wat) { m_name = wat; Usr::save(); }
	
	String getName() { return m_name; }
	u32int getGid() { return m_gid; }
};

#endif
