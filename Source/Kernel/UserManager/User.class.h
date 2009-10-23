#ifndef DEF_USER_CLASS_H
#define DEF_USER_CLASS_H

#include <UserManager/Group.class.h>
#include <Vector.class.h>

class User {
	friend void Usr::load();

	private:
	String m_username, m_completeName;
	u32int m_uid;
	Group* m_group;
	Vector<Group*> m_extraGroups;
	
	User(String username, String completeName, Group* group, String extragroups, u32int uid)
		: m_username(username), m_completeName(completeName), m_uid(uid), m_group(group) {
		Vector<String> eg = extragroups.split(",");
		for (u32int i = 0; i < eg.size(); i++) {
			Group* g = Usr::group(eg[i].toInt());
			if (g != 0) m_extraGroups.push(g);
		}
	}

	public:
	String getUserName() { return m_username; }
	String getCompleteName() { return m_completeName; }
	u32int getUid() { return m_uid; }
	Group* getGroup() { return m_group; }
	bool isInGroup(u32int gid) {
		if (m_group->getGid() == gid) return true;
		for (u32int i = 0; i < m_extraGroups.size(); i++)
			if (m_extraGroups[i]->getGid() == gid) return true;
		return false;
	}
	bool isInGroup(String name) {
		if (m_group->getName() == name) return true;
		for (u32int i = 0; i < m_extraGroups.size(); i++)
			if (m_extraGroups[i]->getName() == name) return true;
		return false;
	}
	bool isInGroup(Group* g) {
		if (g == m_group) return true;
		for (u32int i = 0; i < m_extraGroups.size(); i++)
			if (m_extraGroups[i] == g) return true;
		return false;
	}

	String getGroups() {
		String ret;
		for (u32int i = 0; i < m_extraGroups.size(); i++) {
			if (!ret.empty()) ret += ",";
			ret += String::number(m_extraGroups[i]->getGid());
		}
		return ret;
	}

	void setUserName(String wat) { m_username = wat; Usr::save(); }
	void setCompleteName(String wat) { m_completeName = wat; Usr::save(); }
	void setGroup(Group* group) { m_group = group; Usr::save(); }
	void addGroup(u32int gid) {
		Group* g = Usr::group(gid);
		if (g != 0 and !isInGroup(g)) {
			m_extraGroups.push(g);
		}
		Usr::save();
	}
};

#endif
