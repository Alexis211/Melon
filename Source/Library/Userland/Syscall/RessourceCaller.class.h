#ifndef DEF_RESSOURCECALLER_CLASS_H
#define DEF_RESSOURCECALLER_CLASS_H

#include <Syscall/Syscall.wtf.h>

class RessourceCaller {
	private:
	u32int m_id;
	u32int m_type;

	protected:
	RessourceCaller(u32int id, u32int type);
	static u32int getObjId(u32int type);
	u32int doCall(u8int call, u32int a = 0, u32int b = 0, u32int c = 0, u32int d = 0, u32int e = 0);

	public:
	u32int resId() { return m_id; }
	u32int resType() { return m_type; }
	bool valid() { return m_type != 0; }
};

#endif
