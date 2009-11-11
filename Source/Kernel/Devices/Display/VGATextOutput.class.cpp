#include "VGATextOutput.class.h"
#include <DeviceManager/Disp.ns.h>

#include <TaskManager/V86/V86.ns.h>

//Virtual address in higher half
#define RAM_ADDR 0xC00B8000

using namespace Sys;	//For outb
using namespace Disp;

String VGATextOutput::getClass() {
	return "display.text";
}

String VGATextOutput::getName() {
	return "Standard VGA text display";
}

void VGATextOutput::getModes(Vector<mode_t> &to) {
	mode_t m;
	m.textCols = 40;
	m.textRows = 25;
	m.identifier = 1;
	m.graphWidth = 0;
	m.graphHeight = 0;
	m.device = this;
	to.push(m);
	m.textCols = 80;
	m.identifier = 3;	//3 = text 80x25 16color, just what we want
	to.push(m);
}

bool VGATextOutput::setMode(mode_t& mode) {
	if (mode.device == this && (mode.identifier == 3 or mode.identifier == 1)) {
		v86_regs_t r;
		r.ax = mode.identifier;
		m_cols = mode.textCols;
		V86::biosInt(0x10, r);
		clear();
		return true;
	}
	return false;
}

void VGATextOutput::putChar(u16int line, u16int col, WChar c, u8int color) {
	u16int* where = (u16int*)RAM_ADDR;
	where[(m_cols * line) + col] = (color << 8) | c.toAscii();
}

void VGATextOutput::moveCursor(u16int line, u16int col) {
	u16int csrLoc = (line * m_cols) + col;
	outb(0x3D4, 14);
	outb(0x3D5, csrLoc >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, csrLoc & 0xFF);
}

void VGATextOutput::clear() {
	u16int* where = (u16int*)RAM_ADDR;
	for (int i = 0; i < 25 * 80; i++) where[i] = 0;
}
