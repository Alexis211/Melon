#include "Mutex.class.h"
#include <TaskManager/Task.ns.h>

extern "C" u32int atomic_exchange(u32int* ptr, u32int newval);

Mutex::Mutex(u32int locked) {
	m_locked = locked;
}

bool Mutex::lock() {
	if (atomic_exchange(&m_locked, MUTEX_TRUE) == MUTEX_TRUE) return false;	//The lock was already locked
	return true;
}

void Mutex::waitLock() {
	while (atomic_exchange(&m_locked, MUTEX_TRUE) == MUTEX_TRUE)
		Task::currentThread->sleep(10);	//Wait 10ms
}

void Mutex::unlock() {
	m_locked = MUTEX_FALSE;
}

bool Mutex::locked() {
	return m_locked;
}
