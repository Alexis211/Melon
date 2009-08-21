#ifndef DEF_VGATEXTOUTPUT_CLASS_H
#define DEF_VGATEXTOUTPUT_CLASS_H

#include <Devices/Display/Display.proto.h>

class VGATextOutput : public Display {
	public:
	u16int textCols();
	u16int textRows();
	void putChar(u16int line, u16int col, char c, u8int color);
	void moveCursor(u16int line, u16int col);
	void clear();
};

#endif
