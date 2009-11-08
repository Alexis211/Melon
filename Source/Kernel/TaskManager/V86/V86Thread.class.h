#ifndef DEF_V86THREAD_CLASS_H
#define DEF_V86THREAD_CLASS_H

#include <TaskManager/Thread.class.h>

typedef u32int FARPTR;
#define MK_FP(seg, off) ((FARPTR)(((u32int)(seg) << 16) | (u16int) (off)))
#define FP_SEG(fp) (((FARPTR)fp) >> 16)
#define FP_OFF(fp) (((FARPTR)fp) & 0xFFFF)
#define FP_TO_LINEAR(seg, off) ((void*)((((u16int)(seg)) << 4) + ((u16int)(off))))
inline FARPTR LINEAR_TO_FP(void* ptr) {
	u32int seg, off;
	off = ((size_t) ptr) & 0xF;
	seg = ((size_t) ptr - ((size_t) ptr & 0xF)) / 16;
	return MK_FP(seg, off);
}

struct v86_retval_t {
	registers_t regs;
	bool finished;
};

struct v86_function_t {
	u16int size;
	u8int data[];
};

#define V86_STACKSIZE 1024

// (in segments)
#define V86_ALLOC_START 0x1000
#define V86_ALLOC_END 0x9000

#define EFLAGS_IF 0x200
#define EFLAGS_VM 0x20000
#define VALID_FLAGS 0xDFF

class V86Thread : public Thread {
	private:
	V86Thread();
	V86Thread(V86Thread&);

	v86_retval_t* m_ret;

	bool m_if;	//V86 IF flag

	static u16int allocSeg;
	static u16int alloc(u16int length);	//Returns a segment number

	static void runV86(V86Thread* t, u32int data, u32int ss, u32int cs);
	
	public:
	V86Thread(v86_function_t* entry, v86_retval_t* ret, u32int data);

	bool handleV86GPF(registers_t *regs);
	void handleException(registers_t *regs, int no);	
};

#endif
