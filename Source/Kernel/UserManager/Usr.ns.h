#ifndef DEF_USR_NS_H
#define DEF_USR_NS_H

#include <String.class.h>
class Group;
class User;

namespace Usr {
	void load();	//Loads users into memory, from /System/Configuration/{Users,Groups}
	void save();	//Saves config from mem to filesystem

	u32int uid();	//Returns current processes UID

	User* user(u32int uid);			//Returns user from UID
	User* user(String username);
	User* user();
	Group* group(u32int gid);
	Group* group(String name);

	u32int uid(String username);	//Returns UID of username
};

#endif

#include <UserManager/User.class.h>
