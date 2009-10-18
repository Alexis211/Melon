#ifndef DEF_RESSOURCE_CLASS_H
#define DEF_RESSOURCE_CLASS_H

#include <SimpleList.class.h>

class Ressource;

typedef u32int (Ressource::*call0)();
typedef u32int (Ressource::*call1)(u32int);
typedef u32int (Ressource::*call2)(u32int, u32int);
typedef u32int (Ressource::*call3)(u32int, u32int, u32int);
typedef u32int (Ressource::*call4)(u32int, u32int, u32int, u32int);
typedef u32int (Ressource::*call5)(u32int, u32int, u32int, u32int, u32int);

struct call_t {
	u8int params;
	u8int id;
	union {
		call0 c0;
		call1 c1;
		call2 c2;
		call3 c3;
		call4 c4;
		call5 c5;
	};
};

class Ressource {
	private:
	Ressource(const Ressource&);
	Ressource& operator=(const Ressource&);
	
	u32int m_id;
	u32int m_type;
	SimpleList<call_t> *m_calls;

	protected:
	Ressource(u8int type);
	~Ressource();

	void addCall0(u8int id, call0 c);
	void addCall1(u8int id, call1 c);
	void addCall2(u8int id, call2 c);
	void addCall3(u8int id, call3 c);
	void addCall4(u8int id, call4 c);
	void addCall5(u8int id, call5 c);

	public:
	u32int doCall(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e);
	u32int resId() { return m_id; }
	u32int resType() { return m_type; }
};

#endif
