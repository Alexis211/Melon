#ifndef DEF_V86THREAD_CLASS_H
#define DEF_V86THREAD_CLASS_H

#include <TaskManager/Thread.class.h>

struct v86_retval_t {
	registers_t *regs;
	bool finished;
};

struct v86_function_t {
	u16int size;
	u8int data[];
};

#define V86_STACKSIZE 1024

#define EFLAGS_IF 0x200
#define EFLAGS_VM 0x20000
#define VALID_FLAGS 0xDFF

class V86Thread : public Thread {
	private:
	V86Thread();
	V86Thread(V86Thread&);

	v86_retval_t* m_ret;

	bool m_if;	//V86 IF flag

	static void runV86(V86Thread* t, u32int data, u32int ss, u32int cs);
	
	public:
	V86Thread(v86_function_t* entry, v86_retval_t* ret, u32int data);

	bool handleV86GPF(registers_t *regs);
	void handleException(registers_t *regs, int no);	
};

#endif
