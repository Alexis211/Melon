#include "ScrollableVT.class.h"
#include <VTManager/VT.ns.h>
#include <DeviceManager/Disp.ns.h>

#define BUFCHR(l, c) m_buff[((l) * m_cols) + (c)]

ScrollableVT::ScrollableVT(u32int rows, u32int cols, u32int keepRows, u8int fgcolor, u8int bgcolor) :
		SimpleVT(rows, cols, fgcolor, bgcolor) {
	m_keeprows = keepRows;
	m_linesup = 0;
	m_lines = new vtchr* [keepRows];
	for (u32int i = 0; i < keepRows; i++) {
		m_lines[i] = new vtchr [cols];
		for (u32int j = 0; j < cols; j++) {
			m_lines[i][j].color = m_color;
			m_lines[i][j].c = " ";
		}
	}
}

ScrollableVT::~ScrollableVT() {
	for (u32int i = 0; i < m_keeprows; i++) {
		delete m_lines[i];
	}
	delete m_lines;
}

void ScrollableVT::putChar(u32int row, u32int col, WChar c) {
	if (row >= m_rows or col >= m_cols) return;
	vtchr* ch = &BUFCHR(row, col);
	ch->c = c;
	ch->color = m_color;
	if (m_mapped) {
		if (row + m_linesup < m_rows)
			Disp::putChar(row + m_maprow + m_linesup, col + m_mapcol, BUFCHR(row, col).c, m_color);
	}
}

void ScrollableVT::updateCursor() {
	if (m_csrlin + m_linesup < m_rows)
		Disp::moveCursor(m_csrlin + m_maprow + m_linesup, m_csrcol + m_mapcol);
}

void ScrollableVT::redraw() {
	if (!m_mapped) return;
	for (u32int r = 0; r < m_rows; r++) {
		if (r >= m_linesup) {
			for (u32int c = 0; c < m_cols; c++) {
				Disp::putChar(r + m_maprow, c + m_mapcol, BUFCHR(r - m_linesup, c).c, BUFCHR(r - m_linesup, c).color);
			}
		} else {
			for (u32int c = 0; c < m_cols; c++) {
				register u32int l = m_keeprows - m_linesup + r;
				Disp::putChar(r + m_maprow, c + m_mapcol, m_lines[l][c].c, m_lines[l][c].color);
			}
		}
	}
}

void ScrollableVT::scroll() {
	for (u32int c = 0; c < m_cols; c++) {
		m_lines[0][c] = BUFCHR(0, c);
	}
	vtchr* x = m_lines[0];
	for (u32int l = 1; l < m_keeprows; l++) {
		m_lines[l - 1] = m_lines[l];
	}
	m_lines[m_keeprows - 1] = x;
	SimpleVT::scroll();
}

void ScrollableVT::keyPress(Kbd::keypress_t kp) {
	if (kp.hascmd && kp.modifiers == STATUS_SHIFT) {
		s32int nlup = m_linesup;
		if (kp.command == KBDC_PGUP) {
			nlup = m_linesup + (m_rows - 2);
		} else if (kp.command == KBDC_PGDOWN) {
			nlup = m_linesup - (m_rows - 2);
		} else if (kp.command == KBDC_UP) {
			nlup = m_linesup + 1;
		} else if (kp.command == KBDC_DOWN) {
			nlup = m_linesup - 1;
		} else {
			VirtualTerminal::keyPress(kp);
		}
		if (nlup < 0) nlup = 0;
		m_linesup = nlup;
		if (m_linesup > m_keeprows) m_linesup = m_keeprows;
		redraw();
		updateCursor();
	} else {
		VirtualTerminal::keyPress(kp);
	}
}
