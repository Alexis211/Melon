#ifndef DEF_VIRTUALTERMINAL_CLASS_H
#define DEF_VIRTUALTERMINAL_CLASS_H

#include <common.h>
#include <String.class.h>
#include <Mutex.class.h>
#include <DeviceManager/Kbd.ns.h>
#include <Vector.class.h>

#include <VirtualTerminal.iface.h>

#include <SyscallManager/Ressource.class.h>

struct vtchr {
	u8int color;
	WChar c;
};

#define KBDBUFFSIZE 10

class VirtualTerminal : public Ressource {
	protected:
	Mutex m_kbdMutex;
	keypress_t m_kbdbuff[KBDBUFFSIZE];	//Key press events buffer
	int m_kbdbuffStart, m_kbdbuffEnd;

	//SYSCALLS :
	static call_t m_callTable[];
	u32int writeHexSC(u32int);
	u32int writeDecSC(u32int, u32int);
	u32int writeSC(u32int);
	u32int putSC(u32int);
	u32int readLineSC(u32int);
	u32int getKeypressSC(u32int);
	u32int setColorSC(u32int);
	u32int setCursorLineSC(u32int);
	u32int setCursorColSC(u32int);
	u32int isBoxedSC();
	u32int getHeightSC();
	u32int getWidthSC();
	u32int locateSC(u32int, u32int);
	bool accessible() { return true; }

	//Internal use
	virtual void updateCursor() {}

	public:
	static u32int scall(u8int, u32int, u32int, u32int, u32int);

	VirtualTerminal();
	virtual ~VirtualTerminal();

	virtual bool isBoxed() = 0;
	virtual u8int height() { return 0; }
	virtual u8int width() { return 0; }

	virtual void handleEscape(mvt_esc_cmd_t cmd) = 0;

	//Display functions
	virtual void put(WChar c, bool updatecsr = true) = 0;
	void write(const String& s, bool updatecsr = true);

	//Display functions for kernel use
	void writeDec(s64int num, bool updatecsr = true);
	void writeHex(u32int i, bool updatecsr = true);
	virtual void hexDump(u8int* ptr, u32int sz, bool addnl = true);	//Always ignore parameter addnl
	inline VirtualTerminal& operator<<(const String& s) { write(s); return *this; }
	inline VirtualTerminal& operator<<(s32int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(s64int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(u32int i) { writeHex(i); return *this; }
	inline VirtualTerminal& operator<<(mvt_esc_cmd_t cmd) { handleEscape(cmd); return *this; }

	//Keyboard functions
	virtual void keyPress(keypress_t kp);	//Called by Kbd:: when a key is pressed, overloaded by ScrollableVT
	virtual keypress_t getKeypress(bool show = true, bool block = true);	//Block : must we wait for a key to be pressed ?
	String readLine(bool show = true);
};

#endif
