#include "VirtualTerminal.proto.h"
#include <VirtualTerminal.iface.h>
#include <TaskManager/Task.ns.h>

call_t VirtualTerminal::m_callTable[] = {
	CALL1(VTIF_WRITE, &VirtualTerminal::writeSC),
	CALL1(VTIF_PUT, &VirtualTerminal::putSC),
	CALL1(VTIF_READLINE, &VirtualTerminal::readLineSC),
	CALL1(VTIF_GETKEYPRESS, &VirtualTerminal::getKeypressSC),
	CALL0(VTIF_ISBOXED, &VirtualTerminal::isBoxedSC),
	CALL0(VTIF_GETHEIGHT, &VirtualTerminal::getHeightSC),
	CALL0(VTIF_GETWIDTH, &VirtualTerminal::getWidthSC),
	CALL0(0, 0)
};

u32int VirtualTerminal::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == VTIF_SGETPRINVT) return Task::currProcess()->getInVT()->resId();
	if (wat == VTIF_SGETPROUTVT) return Task::currProcess()->getOutVT()->resId();
	return (u32int) - 1;
}

u32int VirtualTerminal::writeSC(u32int wat) {
	String *s = (String*)wat;
	//hexDump((u8int*)s->serialize(), s->size());
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

u32int VirtualTerminal::isBoxedSC() {
	return (isBoxed() ? 1 : 0);
}

u32int VirtualTerminal::getHeightSC() {
	return height();
}

u32int VirtualTerminal::getWidthSC() {
	return width();
}
