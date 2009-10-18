#include "VirtualTerminal.proto.h"

u32int VirtualTerminal::writeHexSC(u32int number) {
	writeHex(number);
	return 0;
}

u32int VirtualTerminal::writeSC(u32int wat) {
	write(String::unserialize(wat));
	return 0;
}

u32int VirtualTerminal::writeDecSC(u32int n_hi, u32int n_lo) {
	s64int n = ((u64int)n_hi << 32) | n_lo;
	writeDec(n);
	return 0;
}

u32int VirtualTerminal::putSC(u32int code) {
	put(WChar(code));
	return 0;
}

u32int VirtualTerminal::readLineSC() {
	return readLine().serialize();
}
