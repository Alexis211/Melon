#include "Mutex.class.h"
#include <TaskManager/Task.ns.h>

Mutex::Mutex(bool locked) {
	m_locked = locked;
}

bool Mutex::lock() {
	if (m_locked) return false;
	m_locked = true;
	return m_locked;
}

void Mutex::waitLock() {
	while (m_locked)
		Task::currentThread->sleep(10);	//Wait 10ms
	m_locked = true;
}

void Mutex::unlock() {
	m_locked = false;
}

bool Mutex::locked() {
	return m_locked;
}
