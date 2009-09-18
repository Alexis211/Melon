#ifndef DEF_MUTEX_CLASS_H
#define DEF_MUTEX_CLASS_H

#include <Core/common.wtf.h>

#define MUTEX_FALSE 0
#define MUTEX_TRUE 1

class Mutex {
	private:
	u32int m_locked;

	public:
	Mutex(u32int locked = MUTEX_FALSE);
	bool lock();		//Locks the mutex if it is not locked. Returns true if mutex could be locked, false if already locked
	void waitLock();	//Locks the mutex, waiting for it to be unlocked before if necessary
	void unlock();
	bool locked();
};

#endif
