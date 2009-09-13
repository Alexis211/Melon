#include "VirtualTerminal.class.h"
#include <DeviceManager/Disp.ns.h>
#include <VTManager/VT.ns.h>

#define BUFCHR(l, c) m_buff[((l) * m_cols) + (c)]

VirtualTerminal::VirtualTerminal(u32int rows, u32int cols, u8int fgcolor, u8int bgcolor) : m_kbdMutex(false) {
	m_buff = new chr[rows * cols];
	m_rows = rows;
	m_cols = cols;
	m_mapped = false;
	setColor(fgcolor, bgcolor);
	clear();

	m_csrcol = 0;
	m_csrlin = 0;
}

VirtualTerminal::~VirtualTerminal() {
	if (m_mapped) VT::unmap(this);
	delete [] m_buff;
}

void VirtualTerminal::setColor(u8int fgcolor, u8int bgcolor) {
	if (bgcolor == 0xFF) {
		m_color = (m_color & 0xF0) | fgcolor;
	} else {
		m_color = (bgcolor << 4) | fgcolor;	
	}
}

void VirtualTerminal::putChar(u32int row, u32int col, WChar c) {
	if (row >= m_rows or col >= m_cols) return;
	chr* ch = &BUFCHR(row, col);
	ch->c = c;
	ch->color = m_color;
	if (m_mapped) 
		Disp::putChar(row + m_maprow, col + m_mapcol, BUFCHR(row, col).c, m_color);
}

void VirtualTerminal::clear() {
	for (u32int i = 0; i < m_rows * m_cols; i++) {
		m_buff[i].c = ' ';
		m_buff[i].color = m_color;
	}
	if (m_mapped) redraw();
}

void VirtualTerminal::map(s32int row, s32int col) {
	m_maprow = (row == -1 ? (Disp::textRows() / 2) - (m_rows / 2) : row);
	m_mapcol = (col == -1 ? (Disp::textCols() / 2) - (m_cols / 2) : col);
	m_mapped = true;
	redraw();
	VT::map(this);
}

void VirtualTerminal::unmap() {
	m_mapped = false;
	VT::unmap(this);
}

void VirtualTerminal::redraw() {
	if (!m_mapped) return;
	for (u32int r = 0; r < m_rows; r++) {
		for (u32int c = 0; c < m_cols; c++) {
			Disp::putChar(r + m_maprow, c + m_mapcol, BUFCHR(r, c).c, BUFCHR(r, c).color);
		}
	}
}

void VirtualTerminal::scroll() {
	for (u32int l = 0; l < m_rows - 1; l++) {
		for (u32int c = 0; c < m_cols; c++) {
			BUFCHR(l, c) = BUFCHR(l + 1, c);
		}
	}
	for (u32int c = 0; c < m_cols; c++) {
		BUFCHR(m_rows - 1, c).c = ' ';
		BUFCHR(m_rows - 1, c).color = m_color;
	}	
	if (m_mapped) redraw();
}

void VirtualTerminal::updateCursor() {
	Disp::moveCursor(m_csrlin + m_maprow, m_csrcol + m_mapcol);
}

void VirtualTerminal::moveCursor(u32int row, u32int col) {
	m_csrlin = row;
	m_csrcol = col;
	updateCursor();
}

void VirtualTerminal::setCursorLine(u32int line) {
	m_csrlin = line;
	updateCursor();
}

void VirtualTerminal::setCursorCol(u32int col) {
	m_csrcol = col;
	updateCursor();
}


// Display functionn
void VirtualTerminal::put(WChar c, bool updatecsr) {
	if (c.value == '\b') {
		if (m_csrcol > 0) m_csrcol--;
		putChar(m_csrlin, m_csrcol, ' ');
	} else if (c.value == '\t') {
		m_csrcol = (m_csrcol + 8) &~(8 - 1);
	} else if (c.value == '\r') {
		m_csrcol = 0;
	} else if (c.value == '\n') {
		m_csrcol = 0;
		m_csrlin++;
	} else if (c.value >= ' ') {	//Printable character
		putChar(m_csrlin, m_csrcol, c);
		m_csrcol++;
	}
	if (m_csrcol >= m_cols) {
		m_csrcol = 0;
		m_csrlin++;
	}
	while (m_csrlin >= m_rows) {
		scroll();
		m_csrlin--;
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::write(const String& s, bool updatecsr) {
	for (u32int i = 0; i < s.size(); i++) {
		put(s[i], false);
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::writeDec(s32int i, bool updatecsr) {
	if (i == 0) {
		put('0', false);
	} else if (i < 0) {
		put('-', false);
		i = 0 - i;
	}
	char c[32];
	int n = 0;
	while (i > 0) {
		c[n] = '0' + (i % 10);
		i /= 10;
		n++;
	}
	while (n > 0) {
		n--;
		put(c[n], false);
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::writeHex(u32int i, bool updatecsr) {
	write("0x", false);
	char hexdigits[] = "0123456789ABCDEF";
	for (u32int j = 0; j < 8; j++) {
		put(hexdigits[(i & 0xF0000000) >> 28], false);
		i = i << 4;
	}
	if (updatecsr) updateCursor();
}

void VirtualTerminal::hexDump(u8int *ptr, u32int sz) {
	if (m_cols < 76) return;	//Not enough space
	write("HEX Dump, from "); writeHex((u32int)ptr); write("\n");
	char hexdigits[] = "0123456789ABCDEF";
	for (u32int i = 0; i < sz; i += 16) {
		writeHex(i);
		write(" ");
		for (u32int j = 0; j < 16; j++) {
			u8int b = ptr[i + j];
			if (j > 7) put(" ");
			put(hexdigits[b >> 4]);
			put(hexdigits[b & 0xF]);
			if (j < 8) put(" ");
		}
		write(" ");
		for (u32int j = 0; j < 16; j++) {
			u8int b = ptr[i + j];
			if (b >= 0x20 && b < 128) {
				put(WChar(b));
			} else {
				put(".");
			}
		}
		if (m_cols > 76) write("\n");
	}
}
