#ifndef DEF_VIRTUALTERMINAL_CLASS_H
#define DEF_VIRTUALTERMINAL_CLASS_H

#include <Core/common.wtf.h>
#include <Library/String.class.h>

struct chr {
	u8int color;
	wchar c;
	};

class VirtualTerminal {
	private:
	chr* m_buff;
	u32int m_rows, m_cols;
	u8int m_color;

	u32int m_maprow, m_mapcol;
	bool m_mapped;
	
	u32int m_csrlin, m_csrcol;

	public:
	VirtualTerminal(u32int rows, u32int cols, u8int fgcolor = 7, u8int bgcolor = 0);
	~VirtualTerminal();

	void setColor(u8int fgcolor, u8int bgcolor = 0xFF);
	void putChar(u32int row, u32int col, wchar c);
	void clear();

	void map(s32int row = -1, s32int col = -1);
	void unmap();
	void redraw();
	void scroll();	//Scrolls 1 line

	void updateCursor();
	void moveCursor(u32int row, u32int col);
	void setCursorLine(u32int line);
	void setCursorCol(u32int col);

	//Display functions
	void put(wchar c, bool updatecsr = true);
	void write(String s, bool updatecsr = true);
	void writeDec(s32int i, bool updatecsr = true);
	void writeHex(u32int i, bool updatecsr = true);
	
	inline VirtualTerminal& operator<<(String s) { write(s); return *this; }
	//inline VirtualTerminal& operator<<(wchar c) { put(c); return *this; }
	inline VirtualTerminal& operator<<(s32int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(u32int i) { writeHex(i); return *this; }
};

#endif
