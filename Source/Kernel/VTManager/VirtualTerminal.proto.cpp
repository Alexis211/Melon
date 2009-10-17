#include "VirtualTerminal.proto.h"
#include <DeviceManager/Disp.ns.h>
#include <VTManager/VT.ns.h>

VirtualTerminal::VirtualTerminal() : m_kbdMutex(false), m_kbdbuffMutex(false), m_kbdbuff() {
}

VirtualTerminal::~VirtualTerminal() {
}

void VirtualTerminal::write(const String& s, bool updatecsr) {
	for (u32int i = 0; i < s.size(); i++) {
		put(s[i], false);
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::writeDec(s64int num, bool updatecsr) {
	u64int i = num;
	if (i == 0) {
		put('0', false);
	} else if (num < 0) {
		put('-', false);
		i = 0 - num;
	}
	char c[32];
	int n = 0;
	while (i > 0) {
		c[n] = '0' + (i % 10);
		i /= 10;
		n++;
	}
	while (n > 0) {
		n--;
		put(c[n], false);
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::writeHex(u32int i, bool updatecsr) {
	write("0x", false);
	char hexdigits[] = "0123456789ABCDEF";
	for (u32int j = 0; j < 8; j++) {
		put(hexdigits[(i & 0xF0000000) >> 28], false);
		i = i << 4;
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::hexDump(u8int *ptr, u32int sz, bool addnl) {
	write("HEX Dump, from "); writeHex((u32int)ptr); write("\n");
	char hexdigits[] = "0123456789ABCDEF";
	for (u32int i = 0; i < sz; i += 16) {
		writeHex(i);
		write(" ");
		for (u32int j = 0; j < 16; j++) {
			u8int b = ptr[i + j];
			if (j > 7) put(" ");
			put(hexdigits[b >> 4]);
			put(hexdigits[b & 0xF]);
			if (j < 8) put(" ");
		}
		write(" ");
		for (u32int j = 0; j < 16; j++) {
			u8int b = ptr[i + j];
			if (b >= 0x20 && b < 128) {
				put(WChar(b));
			} else {
				put(".");
			}
		}
		if (addnl) write("\n");
	}
}
