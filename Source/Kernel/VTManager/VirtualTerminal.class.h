#ifndef DEF_VIRTUALTERMINAL_CLASS_H
#define DEF_VIRTUALTERMINAL_CLASS_H

#include <Core/common.wtf.h>
#include <Library/String.class.h>
#include <TaskManager/Mutex.class.h>
#include <DeviceManager/Kbd.ns.h>
#include <Library/Vector.class.h>

struct chr {
	u8int color;
	WChar c;
	};

class VirtualTerminal {
	private:
	chr* m_buff;
	u32int m_rows, m_cols;
	u8int m_color;

	u32int m_maprow, m_mapcol;
	bool m_mapped;
	
	u32int m_csrlin, m_csrcol;

	Mutex m_kbdMutex;
	Vector<Kbd::keypress_t> m_kbdbuff;	//Key press events buffer

	public:
	VirtualTerminal(u32int rows, u32int cols, u8int fgcolor = 7, u8int bgcolor = 0);
	~VirtualTerminal();

	void setColor(u8int fgcolor, u8int bgcolor = 0xFF);
	void putChar(u32int row, u32int col, WChar c);
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
	void put(WChar c, bool updatecsr = true);
	void write(String s, bool updatecsr = true);
	void writeDec(s32int i, bool updatecsr = true);
	void writeHex(u32int i, bool updatecsr = true);

	void hexDump(u8int* ptr, u32int sz);
	
	inline VirtualTerminal& operator<<(String s) { write(s); return *this; }
	//inline VirtualTerminal& operator<<(WChar c) { put(c); return *this; }
	inline VirtualTerminal& operator<<(s32int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(u32int i) { writeHex(i); return *this; }

	//Keyboard functions
	void keyPress(Kbd::keypress_t kp);	//Called by Kbd:: when a key is pressed
	Kbd::keypress_t getKeypress(bool show = true, bool block = true);	//Block : must we wait for a key to be pressed ?
	String readLine(bool show = true);
};

#endif
