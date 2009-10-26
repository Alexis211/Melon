#ifndef DEF_RESSOURCE_CLASS_H
#define DEF_RESSOURCE_CLASS_H

#include <SimpleList.class.h>
#include <Mutex.class.h>

class Ressource;

typedef u32int (Ressource::*call0)();
typedef u32int (Ressource::*call1)(u32int);
typedef u32int (Ressource::*call2)(u32int, u32int);
typedef u32int (Ressource::*call3)(u32int, u32int, u32int);
typedef u32int (Ressource::*call4)(u32int, u32int, u32int, u32int);
typedef u32int (Ressource::*call5)(u32int, u32int, u32int, u32int, u32int);

#define CALL0(id, ptr) {0, id, {(call0)ptr}}
#define CALL1(id, ptr) {1, id, {c1: (call1)ptr}}
#define CALL2(id, ptr) {2, id, {c2: (call2)ptr}}
#define CALL3(id, ptr) {3, id, {c3: (call3)ptr}}
#define CALL4(id, ptr) {4, id, {c4: (call4)ptr}}
#define CALL5(id, ptr) {5, id, {c5: (call5)ptr}}

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
	
	Mutex m_lock;

	u32int m_id;
	u32int m_type;
	SimpleList<call_t*> *m_callTables;

	u32int doCall(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e);

	protected:
	Ressource(u8int type, call_t* callTable = 0);
	virtual ~Ressource();

	//This function's role is to tell the Ressource if it is supposed to be accesible from current user or not
	virtual bool accessible() = 0;	//This function should be overloaded by all derivated classes

	void addCallTable(call_t* callTable);

	public:
	u32int call(u8int id, u32int a, u32int b, u32int c, u32int d, u32int e);
	u32int resId() { return m_id; }
	u32int resType() { return m_type; }
};

#endif
