#ifndef DEF_SIMPLEVT_CLASS_H
#define DEF_SIMPLEVT_CLASS_H

#include <VTManager/VirtualTerminal.proto.h>

class SimpleVT : public VirtualTerminal {
	protected:
	vtchr* m_buff;
	u32int m_rows, m_cols;
	u8int m_color;

	u32int m_maprow, m_mapcol;
	bool m_mapped, m_hideCursor;
	
	u32int m_csrlin, m_csrcol;

	public:
	SimpleVT(u32int rows, u32int cols, u8int fgcolor = 7, u8int bgcolor = 0);
	virtual ~SimpleVT();

	virtual void putChar(u32int row, u32int col, WChar c);
	void clear();
	void setColor(u8int fgcolor, u8int bgcolor = 0xFF);
	bool isBoxed() { return true; }
	u8int height() { return m_rows; }
	u8int width() { return m_cols; }
	u8int csrlin() { return m_csrlin; }
	u8int csrcol() { return m_csrcol; }

	void hideCursor(bool hide = true) { m_hideCursor = hide; }

	void map(s32int row = -1, s32int col = -1);
	void unmap();
	virtual void redraw();
	virtual void scroll();	//Scrolls 1 line

	virtual void updateCursor();
	void moveCursor(u32int row, u32int col);
	void setCursorLine(u32int line);
	void setCursorCol(u32int col);

	void put(WChar c, bool updatecsr = true);

	virtual void hexDump(u8int* ptr, u32int sz, bool addnl = false); //Ignore parameter addnl
};

#endif

