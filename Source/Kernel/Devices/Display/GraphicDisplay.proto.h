#ifndef DEF_GRAPHICDISPLAY_PROTO_H
#define DEF_GRAPHICDISPLAY_PROTO_H

#include <Devices/Display/Display.proto.h>

extern char consoleFont[256][16];
extern u32int consoleColor[16];

#define C_FONT_WIDTH 9	//This is width of one character ON SCREEN, real width is 8px (1px padding)
#define C_FONT_HEIGHT 16

class GraphicDisplay : public Display {
	protected:
	struct {
		int line, col;
		u32int buff[C_FONT_WIDTH][C_FONT_HEIGHT];
	} m_csrBuff;

	//Cursor handling
	void getCsrBuff();
	void putCsrBuff();
	void drawCsr();

	public:
	GraphicDisplay() { m_csrBuff.line = -1; m_csrBuff.col = -1; }

	virtual void putChar(u16int line, u16int col, WChar c, u8int color);
	virtual void drawChar(u16int line, u16int col, WChar c, u8int color);

	virtual void moveCursor(u16int line, u16int col);
};

#endif
