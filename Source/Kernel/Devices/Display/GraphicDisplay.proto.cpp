#include "GraphicDisplay.proto.h"

#include <Ressources/ConsoleFont.cd>

u32int consoleColor[16] = {
	0x00000000,		// 0 == black
	0x00000077,		// 1 == dark blue
	0x00007700,		// 2 == dark green
	0x00007777, 	// 3 == dark cyan
	0x00770000,		// 4 == dark red
	0x00770077,		// 5 == dark magenta
	0x00777700,		// 6 == dark orange
	0x00777777,		// 7 == light grey
	0x00444444,		// 8 == dark grey
	0x000000FF,		// 9 == bright blue
	0x0000FF00,		// A == bright green
	0x0000FFFF,		// B == bright cyan
	0x00FF0000,		// C == bright red
	0x00FF00FF,		// D == bright yellow
	0x00FFFF00,		// E == bright orange
	0x00FFFFFF,		// F == white
};

void GraphicDisplay::putChar(u16int line, u16int col, WChar c, u8int color) {
	u8int ch = c.toAscii();
	if (ch == 0) return;

	u16int sx = col * C_FONT_WIDTH, sy = line * C_FONT_HEIGHT;
	u32int fgcolor = consoleColor[color & 0xF], bgcolor = consoleColor[(color >> 4) & 0xF];

	for (int y = 0; y < C_FONT_HEIGHT; y++) {
		u8int pixs = consoleFont[ch][y];
		for (int x = 7; x >= 0; x--) {
			putPix(sx + x, sy + y, ((pixs & 1) != 0 ? fgcolor : bgcolor));
			pixs = pixs >> 1;
		}
		putPix(sx + 8, sy + y, bgcolor);
	}
}

void GraphicDisplay::moveCursor(u16int line, u16int col) {

	//draw some cursor
	for (int x = 0; x < C_FONT_WIDTH; x++) {
		putPix((col * C_FONT_WIDTH) + x, (line * C_FONT_HEIGHT) + 14, 0x00FFFFFF);
		putPix((col * C_FONT_WIDTH) + x, (line * C_FONT_HEIGHT) + 15, 0x00000000);
	}
}
