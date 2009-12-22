#include "V86.ns.h"
#include <TaskManager/Task.ns.h>

#include <MemoryManager/V86Segment.class.h>

namespace V86 {

u16int seg = V86_ALLOC_START;

void run(v86_function_t& entry, v86_regs_t &regs) {
	v86_retval_t ret;
	ret.regs = &regs;
	new V86Thread(&entry, &ret);
	while (!ret.finished) Task::currThread()->sleep(10);
}

void biosInt(u8int int_no, v86_regs_t &regs) {
	v86_retval_t ret;
	ret.regs = &regs;
	new V86Thread(int_no, &ret);
	while (!ret.finished) Task::currThread()->sleep(10);
}

void map(Process *p) {
	if (p == 0) p = Task::currProcess();
	p->getPagedir()->map(&V86Segment::seg);
}

u16int allocSeg(u16int length) {
	if (length & 0xF) length = (length & 0xFFFF0) + 0x10;
	u16int segments = length / 16;
	if (seg < V86_ALLOC_START) seg = V86_ALLOC_START;
	if (seg + segments > V86_ALLOC_END) seg = V86_ALLOC_START;
	u16int ret = seg;
	seg += segments;

	return ret;
}

void* alloc(u16int length) {
	return FP_TO_LINEAR(allocSeg(length), 0);
}

}
