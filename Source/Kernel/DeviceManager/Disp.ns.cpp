#include "Disp.ns.h"

namespace Disp {

mode_t mode;

u16int textCols() {
	return mode.textCols;
}

u16int textRows() {
	return mode.textRows;
}

void putChar(u16int line, u16int col, wchar c, u8int color) {
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

void setDisplay(Display* disp) {
	mode.device = disp;
	disp->clear();
	mode.textCols = disp->textCols();
	mode.textRows = disp->textRows();
}

}
