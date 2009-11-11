#include "GraphicDisplay.proto.h"

#include <Ressources/ConsoleFont.cd>

u32int consoleColor[16] = {
	0x00000000,		// 0 == black
	0x000000AA,		// 1 == dark blue
	0x0000AA00,		// 2 == dark green
	0x0000AAAA, 	// 3 == dark cyan
	0x00AA0000,		// 4 == dark red
	0x00AA00AA,		// 5 == dark magenta
	0x00AA5500,		// 6 == dark orange
	0x00999999,		// 7 == light grey
	0x00555555,		// 8 == dark grey
	0x005555FF,		// 9 == bright blue
	0x0055FF55,		// A == bright green
	0x0055FFFF,		// B == bright cyan
	0x00FF5555,		// C == bright red
	0x00FF55FF,		// D == bright yellow
	0x00FFFF00,		// E == bright orange
	0x00FFFFFF,		// F == white
};

void GraphicDisplay::putChar(u16int line, u16int col, WChar c, u8int color) {
	drawChar(line, col, c, color);
	if (line == m_csrBuff.line and col == m_csrBuff.col) {
		getCsrBuff();
		drawCsr();
	}
}

void GraphicDisplay::drawChar(u16int line, u16int col, WChar c, u8int color) {
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
	putCsrBuff();
	m_csrBuff.line = line; m_csrBuff.col = col;
	getCsrBuff();
	drawCsr();
}

//************ CURSOR HANDLING
void GraphicDisplay::getCsrBuff() {
	if (m_csrBuff.col < 0 or m_csrBuff.line < 0) return;
	for (int x = 0; x < C_FONT_WIDTH; x++) {
		for (int y = 0; y < C_FONT_HEIGHT; y++) {
			m_csrBuff.buff[x][y] = getPix((m_csrBuff.col * C_FONT_WIDTH) + x, (m_csrBuff.line * C_FONT_HEIGHT) + y);
		}
	}
}

void GraphicDisplay::putCsrBuff() {
	if (m_csrBuff.col < 0 or m_csrBuff.line < 0) return;
	for (int x = 0; x < C_FONT_WIDTH; x++) {
		for (int y = 0; y < C_FONT_HEIGHT; y++) {
			 putPix((m_csrBuff.col * C_FONT_WIDTH) + x, (m_csrBuff.line * C_FONT_HEIGHT) + y, m_csrBuff.buff[x][y]);
		}
	}
}

void GraphicDisplay::drawCsr() {
	if (m_csrBuff.col < 0 or m_csrBuff.line < 0) return;
	for (int x = 0; x < C_FONT_WIDTH; x++) {
		putPix((m_csrBuff.col * C_FONT_WIDTH) + x, (m_csrBuff.line * C_FONT_HEIGHT) + 14, 0x00FFFFFF);
		putPix((m_csrBuff.col * C_FONT_WIDTH) + x, (m_csrBuff.line * C_FONT_HEIGHT) + 15, 0x00000000);
	}
}
