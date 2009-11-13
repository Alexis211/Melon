#include "VirtualTerminal.proto.h"
#include <VirtualTerminal.iface.h>
#include <TaskManager/Task.ns.h>

call_t VirtualTerminal::m_callTable[] = {
	CALL1(VTIF_WRITEHEX, &VirtualTerminal::writeHexSC),
	CALL2(VTIF_WRITEDEC, &VirtualTerminal::writeDecSC),
	CALL1(VTIF_WRITE, &VirtualTerminal::writeSC),
	CALL1(VTIF_PUT, &VirtualTerminal::putSC),
	CALL1(VTIF_READLINE, &VirtualTerminal::readLineSC),
	CALL1(VTIF_GETKEYPRESS, &VirtualTerminal::getKeypressSC),
	CALL1(VTIF_SETCOLOR, &VirtualTerminal::setColorSC),
	CALL1(VTIF_SETCSRLINE, &VirtualTerminal::setCursorLineSC),
	CALL1(VTIF_SETCSRCOL, &VirtualTerminal::setCursorColSC),
	CALL0(VTIF_ISBOXED, &VirtualTerminal::isBoxedSC),
	CALL0(VTIF_GETHEIGHT, &VirtualTerminal::getHeightSC),
	CALL0(VTIF_GETWIDTH, &VirtualTerminal::getWidthSC),
	CALL2(VTIF_LOCATE, &VirtualTerminal::locateSC),
	CALL0(0, 0)
};

u32int VirtualTerminal::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == VTIF_SGETPRINVT) return Task::currProcess()->getInVT()->resId();
	if (wat == VTIF_SGETPROUTVT) return Task::currProcess()->getOutVT()->resId();
	return (u32int) - 1;
}

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

u32int VirtualTerminal::readLineSC(u32int show) {
	return readLine(show != 0).serialize();
}

u32int VirtualTerminal::getKeypressSC(u32int flags) {
	bool show = (flags & 1), block = (flags & 2);
	keypress_t temp = getKeypress(show, block);
	void* ptr = Mem::mkXchgSpace(sizeof(keypress_t));
	memcpy((u8int*)ptr, (const u8int*)&temp, sizeof(keypress_t));
	return (u32int)ptr;
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

u32int VirtualTerminal::getHeightSC() {
	return height();
}

u32int VirtualTerminal::getWidthSC() {
	return width();
}

u32int VirtualTerminal::locateSC(u32int line, u32int col) {
	if (line < 1000 and col < 1000) {
		moveCursor(line, col);
	} else if (line < 1000) {
		setCursorLine(line);
	} else if (col < 1000) {
		setCursorCol(line);
	}
	return 0;
}
