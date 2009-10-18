#ifndef DEF_VIRTUALTERMINAL_CLASS_H
#define DEF_VIRTUALTERMINAL_CLASS_H

#include <common.h>
#include <String.class.h>
#include <TaskManager/Mutex.class.h>
#include <DeviceManager/Kbd.ns.h>
#include <Vector.class.h>

struct vtchr {
	u8int color;
	WChar c;
};

class VirtualTerminal {
	protected:
	Mutex m_kbdMutex, m_kbdbuffMutex;
	Vector<Kbd::keypress_t> m_kbdbuff;	//Key press events buffer

	public:
	VirtualTerminal();
	virtual ~VirtualTerminal();

	virtual void setColor(u8int fgcolor, u8int bgcolor = 0xFF) {}	//For a pipe/file VT, this will do nothing.
	virtual bool isBoxed() = 0;

	virtual void updateCursor() {}
	virtual void moveCursor(u32int row, u32int col) {}	//These are not implemented for pipe/file VTs
	virtual void setCursorLine(u32int line) {}
	virtual void setCursorCol(u32int col) {}	//This one could be, and should be. It's used a lot for tabulating, etc.

	//Display functions
	virtual void put(WChar c, bool updatecsr = true) = 0;
	void write(const String& s, bool updatecsr = true);
	void writeDec(s64int num, bool updatecsr = true);
	void writeHex(u32int i, bool updatecsr = true);

	virtual void hexDump(u8int* ptr, u32int sz, bool addnl = true);	//Always ignore parameter addnl
	
	inline VirtualTerminal& operator<<(const String& s) { write(s); return *this; }
	inline VirtualTerminal& operator<<(s32int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(s64int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(u32int i) { writeHex(i); return *this; }

	//Keyboard functions
	virtual void keyPress(Kbd::keypress_t kp);	//Called by Kbd:: when a key is pressed, overloaded by ScrollableVT
	virtual Kbd::keypress_t getKeypress(bool show = true, bool block = true);	//Block : must we wait for a key to be pressed ?
	String readLine(bool show = true);
};

#endif
