#include "VGATextOutput.class.h"

using namespace Sys;	//For outb

u16int VGATextOutput::textCols() {
	return 80;
}

u16int VGATextOutput::textRows() {
	return 25;
}

void VGATextOutput::putChar(u16int line, u16int col, char c, char color) {
	u16int* where = (u16int*)0xB8000;
	where[(80 * line) + col] = (color << 8) | c;
}

void VGATextOutput::moveCursor(u16int line, u16int col) {
	u16int csrLoc = (line * 80) + col;
	outb(0x3D4, 14);
	outb(0x3D5, csrLoc >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, csrLoc & 0xFF);
}

void VGATextOutput::clear() {
	u16int* where = (u16int*)0xB8000;
	for (int i = 0; i < 25 * 80; i++) where[i] = 0;
}
