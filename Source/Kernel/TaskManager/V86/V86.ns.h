#ifndef DEF_V86_NS_H
#define DEF_V86_NS_H

#include <TaskManager/V86/V86Thread.class.h>

//For tweaking with far/linear pointers
typedef u32int FARPTR;
#define MK_FP(seg, off) ((FARPTR)(((u32int)(seg) << 16) | (u16int) (off)))
#define FP_SEG(fp) (((FARPTR)fp) >> 16)
#define FP_OFF(fp) (((FARPTR)fp) & 0xFFFF)
#define LIN_SEG(ptr) (((size_t) ptr >> 4) & 0xFFFF)
#define LIN_OFF(ptr) (((size_t) ptr) & 0xF)
#define FP_TO_LINEAR(seg, off) ((void*)((((u16int)(seg)) << 4) + ((u16int)(off))))
inline FARPTR LINEAR_TO_FP(void* ptr) {
	u32int seg, off;
	off = LIN_OFF(ptr);
	seg = LIN_SEG(ptr);
	return MK_FP(seg, off);
}

// (in segments)
#define V86_ALLOC_START 0x1000
#define V86_ALLOC_END 0x9000

namespace V86 {
	void run(v86_function_t& entry, v86_regs_t &regs);
	void biosInt(u8int int_no, v86_regs_t &regs);

	void map(Process* p = 0);

	u16int allocSeg(u16int size);
	void* alloc(u16int size);
}

#endif
