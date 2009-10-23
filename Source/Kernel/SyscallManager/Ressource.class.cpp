#include "Ressource.class.h"
#include <SyscallManager/Res.ns.h>
#include <UserManager/Usr.ns.h>

Ressource::Ressource(u8int type, call_t* callTable) : m_lock(MUTEX_FALSE) {
	m_id = Res::registerRes(this);
	m_type = type;
	m_callTables = 0;

	addCallTable(callTable);
}

Ressource::~Ressource() {
	Res::unregisterRes(m_id);
	delete m_callTables;
}

void Ressource::addCallTable(call_t* callTable) {
	if (callTable != 0) m_callTables = m_callTables->cons(callTable);
}

u32int Ressource::doCall(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e) {
	if (id == 0) return m_type;

	for (SimpleList<call_t*> *iter = m_callTables; iter != 0; iter = iter->next()) {
		call_t* ct = iter->v();
		u32int i = 0;
		while (ct[i].id != 0) {
			call_t &ce = ct[i];
			if (ce.id == id) {
				if (ce.params == 0) return (this->*(ce.c0))();
				if (ce.params == 1) return (this->*(ce.c1))(a);
				if (ce.params == 2) return (this->*(ce.c2))(a, b);
				if (ce.params == 3) return (this->*(ce.c3))(a, b, c);
				if (ce.params == 4) return (this->*(ce.c4))(a, b, c, d);
				if (ce.params == 5) return (this->*(ce.c5))(a, b, c, d, e);
			}
			i++;
		}
	}
	return (u32int) - 1;
}

u32int Ressource::call(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e) {
	if (!ISROOT && !accessible()) return (u32int) - 1;
	m_lock.waitLock();
	u32int r = doCall(id, a, b, c, d, e);
	m_lock.unlock();
	return r;
}
