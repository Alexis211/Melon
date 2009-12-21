#include "Disp.ns.h"
#include <DeviceManager/Dev.ns.h>
#include <VTManager/VT.ns.h>
#include <VTManager/ScrollableVT.class.h>
#include <Core/SB.ns.h>

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

bool textScroll(u16int line, u16int col, u16int height, u16int width, u8int color) {
	return mode.device->textScroll(line, col, height, width, color);
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
	mode.device->unsetMode();

	if (newmode.device->setMode(newmode)) {
		mode = newmode;
		VT::redrawScreen();
		return true;
	}
	return false;
}

void selectMode() {
	getModes();
	*kvt << "\nPlease select a graphic mode in the list below:\n";

	for (u32int i = 0; i < Disp::modes.size(); i++) {
		Disp::mode_t& m = Disp::modes[i];
		*kvt << (s32int)i << ":\t" << "Text " << m.textRows << "x" << m.textCols;
		kvt->setCursorCol(21);
		if (m.graphWidth != 0 and m.graphHeight != 0) {
			*kvt << "Graphics " << m.graphWidth << "x" << m.graphHeight << "x" << m.graphDepth << "\t";
		} else {
			*kvt << "No graphics";
		}
		kvt->setCursorCol(45);
		*kvt << m.device->getName() << "\n";
	}

	while (1) {
		*kvt << "\nYour selection: ";
		String answer = kvt->readLine();
		u32int n = answer.toInt();
		kvt->unmap();
		if (n >= 0 and n < Disp::modes.size() and Disp::setMode(Disp::modes[n])) {
			delete kvt;
			SB::reinit();
			kvt = new ScrollableVT(Disp::mode.textRows - SB::height, Disp::mode.textCols, 100, KVT_FGCOLOR, KVT_BGCOLOR);
			kvt->map(SB::height);
			Kbd::setFocus(kvt);
			return;
		} else {
			Disp::setMode(Disp::modes[1]);
			kvt->map();
			*kvt << "Error while switching video mode, please select another one.";
		}
	}
}

void setText(VGATextOutput* o) {
	mode.device = o;
	o->clear();
	mode.textCols = 80;
	mode.textRows = 25;
}

}
