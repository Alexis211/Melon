#include "VirtualTerminal.proto.h"

u32int VirtualTerminal::writeHexSC(u32int number) {
	writeHex(number);
	return 0;
}

u32int VirtualTerminal::writeDecSC(u32int n_hi, u32int n_lo) {
	s64int n = ((u64int)n_hi << 32) | n_lo;
	writeDec(n);
	return 0;
}

u32int VirtualTerminal::writeSC(u32int wat) {
	String *s = (String*)wat;
	write(*s);
	return 0;
}

u32int VirtualTerminal::putSC(u32int code) {
	put(WChar(code));
	return 0;
}

u32int VirtualTerminal::readLineSC() {
	return readLine().serialize();
}

u32int VirtualTerminal::setColorSC(u32int x) {
	setColor((x >> 8) & 0xFF, x & 0xFF);
	return 0;
}

u32int VirtualTerminal::setCursorLineSC(u32int l) {
	setCursorLine(l);
	return 0;
}

u32int VirtualTerminal::setCursorColSC(u32int c) {
	setCursorCol(c);
	return 0;
}

u32int VirtualTerminal::isBoxedSC() {
	return (isBoxed() ? 1 : 0);
}
