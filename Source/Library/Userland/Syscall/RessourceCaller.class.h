#ifndef DEF_RESSOURCECALLER_CLASS_H
#define DEF_RESSOURCECALLER_CLASS_H

#include <Syscall/Syscall.wtf.h>
#include <common.h>

class Serialized {
	private:
	u32int m_value;

	public:
	Serialized(u32int v) : m_value(v) {}
	~Serialized() { Mem::free( (void*)m_value); }
	operator u32int () { return m_value; }
};

class RessourceCaller {
	private:
	u32int m_id;
	u32int m_type;

	protected:
	RessourceCaller(u32int id, u32int type);

	//Static call -- a call specific to a class and not an object
	static u32int sCall(u32int type, u8int wat, u32int a = 0, u32int b = 0, u32int c = 0, u32int d = 0);	

	u32int doCall(u8int call, u32int a = 0, u32int b = 0, u32int c = 0, u32int d = 0, u32int e = 0);

	public:
	u32int resId() { return m_id; }
	u32int resType() { return m_type; }
	bool valid() { return m_type != 0; }
};

#endif
