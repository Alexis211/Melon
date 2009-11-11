#ifndef DEF_V86THREAD_CLASS_H
#define DEF_V86THREAD_CLASS_H

#include <TaskManager/Thread.class.h>

struct v86_function_t {
	u16int size;
	u8int data[];
};

struct v86_regs_t {
	u16int ax, bx, cx, dx, di, si;
	u16int cs, ds, es, fs, gs, ss;
	u16int ip, sp;
} __attribute__((packed));

struct v86_retval_t {
	v86_regs_t *regs;
	bool finished;
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

	bool m_if, m_continueOnIret;	//V86 IF flag

	void setup();

	public:
	V86Thread(v86_function_t* entry, v86_retval_t* ret);
	V86Thread(u8int int_no, v86_retval_t* ret);

	bool handleV86GPF(registers_t *regs);
	void handleException(registers_t *regs, int no);	
};

#endif
