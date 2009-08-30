#ifndef DEF_MUTEX_CLASS_H
#define DEF_MUTEX_CLASS_H

class Mutex {
	private:
	bool m_locked;

	public:
	Mutex(bool locked = false);
	bool lock();		//Locks the mutex if it is not locked. Returns true if mutex could be locked, false if already locked
	void waitLock();	//Locks the mutex, waiting for it to be unlocked before if necessary
	void unlock();
	bool locked();
};

#endif
