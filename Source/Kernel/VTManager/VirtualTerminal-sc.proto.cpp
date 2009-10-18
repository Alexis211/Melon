#include "VirtualTerminal.proto.h"

u32int VirtualTerminal::writeHexSC(u32int number) {
	writeHex(number);
	return 0;
}

u32int VirtualTerminal::putSC(u32int code) {
	put(WChar(code));
	return 0;
}
