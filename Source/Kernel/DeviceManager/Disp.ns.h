#ifndef DEF_DISP_NS_H
#define DEF_DISP_NS_H

#include <Devices/Display/VGATextOutput.class.h>
#include <WChar.class.h>
#include <Vector.class.h>

namespace Disp {
	struct mode_t {
		int textCols, textRows;
		int graphWidth, graphHeight, graphDepth;
		int identifier;	//Used by video devices
		Display *device;
	};

	extern Vector<mode_t> modes;
	extern mode_t mode;

	u16int textCols();
	u16int textRows();
	void putChar(u16int line, u16int col, WChar c, u8int color);
	void moveCursor(u16int line, u16int col);
	bool textScroll(u16int line, u16int col, u16int height, u16int width, u8int color);
	void clear();

	void getModes();
	bool setMode(mode_t& mode);

	void setText(VGATextOutput* o);	//To use only once : when display is initializing
}

#endif
