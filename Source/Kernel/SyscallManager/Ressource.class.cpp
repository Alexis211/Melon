#include "Ressource.class.h"
#include <SyscallManager/Res.ns.h>

Ressource::Ressource(u8int type) {
	m_id = Res::registerRes(this);
	m_type = type;
	m_calls = 0;

	addCall0(0, (call0)&Ressource::resType);
}

Ressource::~Ressource() {
	Res::unregisterRes(m_id);
	delete m_calls;
}

void Ressource::addCall0(u8int id, call0 c) {
	call_t e = {0, id, {c}};
	m_calls = m_calls->cons(e);
}

void Ressource::addCall1(u8int id, call1 c) {
	call_t e = {1, id, {0}};
	e.c1 = c;
	m_calls = m_calls->cons(e);
}

void Ressource::addCall2(u8int id, call2 c) {
	call_t e = {2, id, {0}};
	e.c2 = c;
	m_calls = m_calls->cons(e);
}

void Ressource::addCall3(u8int id, call3 c) {
	call_t e = {3, id, {0}};
	e.c3 = c;
	m_calls = m_calls->cons(e);
}

void Ressource::addCall4(u8int id, call4 c) {
	call_t e = {4, id, {0}};
	e.c4 = c;
	m_calls = m_calls->cons(e);
}

void Ressource::addCall5(u8int id, call5 c) {
	call_t e = {5, id, {0}};
	e.c5 = c;
	m_calls = m_calls->cons(e);
}

u32int Ressource::doCall(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e) {
	for (SimpleList<call_t> *iter = m_calls; iter != 0; iter = iter->next()) {
		call_t &ce = iter->v();
		if (ce.id == id) {
			if (ce.params == 0) return (this->*(ce.c0))();
			if (ce.params == 1) return (this->*(ce.c1))(a);
			if (ce.params == 2) return (this->*(ce.c2))(a, b);
			if (ce.params == 3) return (this->*(ce.c3))(a, b, c);
			if (ce.params == 4) return (this->*(ce.c4))(a, b, c, d);
			if (ce.params == 5) return (this->*(ce.c5))(a, b, c, d, e);
		}
	}
	return (u32int) - 1;
}
