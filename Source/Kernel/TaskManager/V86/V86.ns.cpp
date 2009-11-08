#include "V86.ns.h"
#include <TaskManager/Task.ns.h>

namespace V86 {

u16int seg = V86_ALLOC_START;

void run(v86_function_t& entry, registers_t &regs, u32int data) {
	v86_retval_t ret;
	ret.regs = &regs;
	new V86Thread(&entry, &ret, data);
	while (!ret.finished) Task::currThread()->sleep(10);
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

void* alloc(u32int length) {
	return FP_TO_LINEAR(allocSeg(length), 0);
}

}
