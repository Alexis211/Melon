#include "RessourceCaller.class.h"

RessourceCaller::RessourceCaller(u32int id, u32int type) {
	m_id = id;
	m_type = 1;
	m_type = doCall(0);
	if (m_type != type) m_type = 0;
}

u32int RessourceCaller::getObjId(u32int type) {
	return syscall(0xFFFFFE00, type);
}

u32int RessourceCaller::doCall(u8int call, u32int a, u32int b, u32int c, u32int d, u32int e) {
	if (m_type == 0) return (u32int) - 1;	//Type 0 = invalid object
	u32int x = ((m_id << 8) | call);
	return syscall(x, a, b, c, d, e);
}
