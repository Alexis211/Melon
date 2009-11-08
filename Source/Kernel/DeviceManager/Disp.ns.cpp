#include "Disp.ns.h"
#include <DeviceManager/Dev.ns.h>

namespace Disp {

mode_t mode;
Vector<mode_t> modes;

u16int textCols() {
	return mode.textCols;
}

u16int textRows() {
	return mode.textRows;
}

void putChar(u16int line, u16int col, WChar c, u8int color) {
	if (line >= mode.textRows or col >= mode.textCols) return;
	mode.device->putChar(line, col, c, color);
}

void moveCursor(u16int line, u16int col) {
	if (line >= mode.textRows or col >= mode.textCols) return;
	mode.device->moveCursor(line, col);
}

void clear() {
	mode.device->clear();
}

void getModes() {
	modes.clear();
	Vector<Device*> disps = Dev::findDevices("display");
	for (u32int i = 0; i < disps.size(); i++) {
		((Display*)(disps[i]))->getModes(modes);
	}
}

bool setMode(mode_t& newmode) {
	if (newmode.device->setMode(newmode)) {
		mode = newmode;
		return true;
	}
	return false;
}

void setText(VGATextOutput* o) {
	mode.device = o;
	o->clear();
	mode.textCols = 80;
	mode.textRows = 25;
}

}
