#include <Core/common.wtf.h>

namespace Mem {

bool kheapUsable = false;
u32int placementAddress, kheapFree;

void *kallocInternal(u32int sz, bool align) {
	if (align && (placementAddress & 0xFFFFF000)) {
		placementAddress &= 0xFFFFF000;
		placementAddress += 0x1000;
	}
	u32int temp = placementAddress;
	placementAddress += sz;
	return (void*)temp;
}

void *kalloc(u32int sz, bool align) {
	if (!kheapUsable) return kallocInternal(sz, align);
	return 0;
}

}
