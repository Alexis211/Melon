#include "SimpleVT.class.h"
#include <VTManager/VT.ns.h>
#include <DeviceManager/Disp.ns.h>

#define BUFCHR(l, c) m_buff[((l) * m_cols) + (c)]

SimpleVT::SimpleVT(u32int rows, u32int cols, u8int fgcolor, u8int bgcolor) : VirtualTerminal() {
	m_buff = new vtchr[rows * cols];
	m_rows = rows;
	m_cols = cols;
	m_mapped = false;
	m_hideCursor = false;
	m_color = ((bgcolor & 0x0F) << 4) | (fgcolor & 0x0F);	
	clear();

	m_csrcol = 0;
	m_csrlin = 0;
}

SimpleVT::~SimpleVT() {
	if (m_mapped) VT::unmap(this);
	delete [] m_buff;
}

void SimpleVT::putChar(u32int row, u32int col, WChar c) {
	if (row >= m_rows or col >= m_cols) return;
	vtchr* ch = &BUFCHR(row, col);
	ch->c = c;
	ch->color = m_color;
	if (m_mapped) 
		Disp::putChar(row + m_maprow, col + m_mapcol, BUFCHR(row, col).c, m_color);
}

void SimpleVT::clear() {
	for (u32int i = 0; i < m_rows * m_cols; i++) {
		m_buff[i].c = ' ';
		m_buff[i].color = m_color;
	}
	if (m_mapped) redraw();
}

void SimpleVT::map(s32int row, s32int col) {
	m_maprow = (row == -1 ? (Disp::textRows() / 2) - (m_rows / 2) : row);
	m_mapcol = (col == -1 ? (Disp::textCols() / 2) - (m_cols / 2) : col);
	m_mapped = true;
	redraw();
	VT::map(this);
}

void SimpleVT::unmap() {
	m_mapped = false;
	VT::unmap(this);
}

void SimpleVT::redraw() {
	if (!m_mapped) return;
	for (u32int r = 0; r < m_rows; r++) {
		for (u32int c = 0; c < m_cols; c++) {
			Disp::putChar(r + m_maprow, c + m_mapcol, BUFCHR(r, c).c, BUFCHR(r, c).color);
		}
	}
}

void SimpleVT::scroll() {
	for (u32int l = 0; l < m_rows - 1; l++) {
		for (u32int c = 0; c < m_cols; c++) {
			BUFCHR(l, c) = BUFCHR(l + 1, c);
		}
	}
	for (u32int c = 0; c < m_cols; c++) {
		BUFCHR(m_rows - 1, c).c = ' ';
		BUFCHR(m_rows - 1, c).color = m_color;
	}	
	if (m_mapped) {
		if (!Disp::textScroll(m_maprow, m_mapcol, m_rows, m_cols, m_color)) {
			redraw();
		}
	}
}

void SimpleVT::updateCursor() {
	if (!m_mapped) return;
	if (m_hideCursor) return;
	Disp::moveCursor(m_csrlin + m_maprow, m_csrcol + m_mapcol);
}

//Command handling
void SimpleVT::handleEscape(mvt_esc_cmd_t cmd) {
	switch (cmd.cmd) {
		case MVTCMD_CLEAR:
			clear();
			break;
		case MVTCMD_SCROLL:
			scroll();
			break;
		case MVTCMD_SETFGCOLOR:
			m_color = (m_color & 0xF0) | (cmd.a & 0x0F);
			break;
		case MVTCMD_SETBGCOLOR:
			m_color = (m_color & 0x0F) | ((cmd.a & 0x0F) << 4);
			break;
		case MVTCMD_SETCOLOR:
			m_color = ((cmd.b & 0x0F) << 4) | (cmd.a & 0x0F);	
			break;
		case MVTCMD_MOVECSR:
			m_csrlin = cmd.a; m_csrcol = cmd.b;
			updateCursor();
			break;
		case MVTCMD_SETCSRLIN:
			m_csrlin = cmd.a;
			updateCursor();
			break;
		case MVTCMD_SETCSRCOL:
			m_csrcol = cmd.a;
			updateCursor();
			break;
		case MVTCMD_HIDECSR:
			m_hideCursor = true;
			break;
		case MVTCMD_SHOWCSR:
			m_hideCursor = false;
			break;
	}
}

// Display functionn
void SimpleVT::put(WChar c, bool updatecsr) {
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
	if (updatecsr && m_mapped) updateCursor();
}

void SimpleVT::hexDump(u8int *ptr, u32int sz, bool addnl) {
	if (m_cols < 76) return;	//Not enough space
	VirtualTerminal::hexDump(ptr, sz, (m_cols > 76));
}
