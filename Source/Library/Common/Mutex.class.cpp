#include "Mutex.class.h"

#ifdef THIS_IS_MELON_KERNEL
#include <TaskManager/Task.ns.h>
#endif

#ifdef THIS_IS_MELON_USERLAND
#include <Binding/Thread.class.h>
#endif

u32int atomic_exchange(u32int* ptr, u32int newval) {
	u32int r;
	asm volatile("xchg (%%ecx), %%eax" : "=a"(r) : "c"(ptr), "a"(newval));
	return r;
}

Mutex::Mutex(u32int locked) {
	m_locked = locked;
}

bool Mutex::lock() {
	if (atomic_exchange(&m_locked, MUTEX_TRUE) == MUTEX_TRUE) return false;	//The lock was already locked
	return true;
}

void Mutex::waitLock() {
	while (atomic_exchange(&m_locked, MUTEX_TRUE) == MUTEX_TRUE) {
#ifdef THIS_IS_MELON_KERNEL
		if (Task::currThread() != 0) Task::currThread()->sleep(10);	//Wait 10ms
		else return;
#endif

#ifdef THIS_IS_MELON_USERLAND
		Thread::get().sleep(10);
#endif
	}
}

void Mutex::unlock() {
	m_locked = MUTEX_FALSE;
}

bool Mutex::locked() {
	return m_locked;
}
