#include "Usr.ns.h"

#include <UserManager/User.class.h>
#include <SimpleList.class.h>
#include <TaskManager/Task.ns.h>
#include <VFS/TextFile.class.h>

/*
 * Syntax for Users and Groups configuration files : one entry per line
 * syntax for Users : <uid>:<username>:<basegroup>:<extragroup>,<extragroup>:<completename>
 * syntax for Groups : <gid>:<name>
 */

namespace Usr {

SimpleList <User> *m_users = 0;
SimpleList <Group> *m_groups = 0;

void load() {
	if (m_users != 0) delete m_users;
	if (m_groups != 0) delete m_groups;
	m_users = 0, m_groups = 0;

	TextFile groups("/System/Configuration/Groups", FM_READ);
	while (!groups.eof()) {
		String s = groups.readLine();
		Vector<String> data = s.split(":");
		if (data.size() == 2 and !(s[0] == WChar("#"))) {
			m_groups = m_groups->cons(Group(data[1], data[0].toInt()));
		}
	}

	TextFile users("/System/Configuration/Users", FM_READ);
	while (!users.eof()) {
		String s = users.readLine();
		Vector<String> data = s.split(":");
		if (data.size() == 5 and !(s[0] == WChar("#"))) {
			m_users = m_users->cons(User(data[1], data[4], group(data[2].toInt()), data[3], data[0].toInt()));
		}
	}
}

void save() {
	TextFile groups("/System/Configuration/Groups", FM_TRUNCATE);
	for (SimpleList<Group> *iter = m_groups; iter != 0; iter = iter->next()) {
		groups.write(String::number(iter->v().getGid()) + ":" + iter->v().getName(), true);
	}
	TextFile users("/System/Configuration/Users", FM_TRUNCATE);
	for (SimpleList<User> *iter = m_users; iter != 0; iter = iter->next()) {
		users.write(String::number(iter->v().getUid()) + ":" + iter->v().getUserName() + ":"
				+ String::number(iter->v().getGroup()->getGid()) + ":" 
				+ iter->v().getGroups() + ":" + iter->v().getCompleteName(), true);
	}
}

u32int uid() {
	return Task::currProcess()->getUid();
}

User* user(u32int uid) {
	for (SimpleList<User> *iter = m_users; iter != 0; iter = iter->next()) {
		if (iter->v().getUid() == uid) return &iter->v();
	}
	return 0;
}

User* user(String username) {
	for (SimpleList<User> *iter = m_users; iter != 0; iter = iter->next()) {
		if (iter->v().getUserName() == username) return &iter->v();
	}
	return 0;
}

User* user() { return user(uid()); }

Group* group(u32int gid) {
	for (SimpleList<Group> *iter = m_groups; iter != 0; iter = iter->next()) {
		if (iter->v().getGid() == gid) return &iter->v();
	}
	return 0;
}

Group* group(String name) {
	for (SimpleList<Group> *iter = m_groups; iter != 0; iter = iter->next()) {
		if (iter->v().getName() == name) return &iter->v();
	}
	return 0;
}

u32int uid(String username) {
	User* x = user(username);
	return (x != 0 ? x->getUid() : (u32int) - 1);
}

};
