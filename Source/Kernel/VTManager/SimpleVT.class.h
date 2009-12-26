#ifndef DEF_SIMPLEVT_CLASS_H
#define DEF_SIMPLEVT_CLASS_H

#include <VTManager/VirtualTerminal.proto.h>

namespace VT {
	void redrawScreen();
}

class SimpleVT : public VirtualTerminal {
	friend void VT::redrawScreen();

	protected:
	vtchr* m_buff;
	u32int m_rows, m_cols;
	u8int m_color;

	u32int m_maprow, m_mapcol;
	bool m_mapped, m_hideCursor;
	
	u32int m_csrlin, m_csrcol;

	virtual void updateCursor();
	virtual void clear();
	virtual void redraw();
	virtual void scroll();	//Scrolls 1 line

	public:
	SimpleVT(u32int rows, u32int cols, u8int fgcolor = 7, u8int bgcolor = 0);
	virtual ~SimpleVT();

	virtual void putChar(u32int row, u32int col, WChar c);

	virtual void handleEscape(mvt_esc_cmd_t cmd);

	bool isBoxed() { return true; }
	u8int height() { return m_rows; }
	u8int width() { return m_cols; }
	u8int csrlin() { return m_csrlin; }
	u8int csrcol() { return m_csrcol; }

	void map(s32int row = -1, s32int col = -1);
	void unmap();

	void put(WChar c, bool updatecsr = true);

	virtual void hexDump(u8int* ptr, u32int sz, bool addnl = false); //Ignore parameter addnl
};

#endif

