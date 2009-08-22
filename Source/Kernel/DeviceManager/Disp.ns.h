#ifndef DEF_DISP_NS_H
#define DEF_DISP_NS_H

#include <Devices/Display/Display.proto.h>

namespace Disp {
	struct mode_t {
		int textCols, textRows;
		Display *device;
	};

	u16int textCols();
	u16int textRows();
	void putChar(u16int line, u16int col, char c, u8int color);
	void moveCursor(u16int line, u16int col);
	void clear();

	void setDisplay(Display* disp);
}

#endif
