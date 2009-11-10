#ifndef DEF_GRAPHICDISPLAY_PROTO_H
#define DEF_GRAPHICDISPLAY_PROTO_H

#include <Devices/Display/Display.proto.h>

extern char consoleFont[256][16];
extern u32int consoleColor[16];

#define C_FONT_WIDTH 9	//This is width of one character ON SCREEN, real width is 8px (1px padding)
#define C_FONT_HEIGHT 16

class GraphicDisplay : public Display {
	struct {
		int x, y;
		u32int buff[C_FONT_WIDTH][C_FONT_HEIGHT];
	} m_csrbuff;

	public:
	GraphicDisplay() { m_csrbuff.x = -1; m_csrbuff.y = -1; }
	virtual void putChar(u16int line, u16int col, WChar c, u8int color);
	virtual void moveCursor(u16int line, u16int col);
};

#endif
