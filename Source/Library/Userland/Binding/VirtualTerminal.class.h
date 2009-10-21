#include <Syscall/RessourceCaller.class.h>

#include <VirtualTerminal.iface.h>

#include <String.class.h>
#include <WChar.class.h>

class VirtualTerminal : public RessourceCaller {
	public:
	static VirtualTerminal getIn() {
		u32int id = RessourceCaller::sCall(VTIF_OBJTYPE, VTIF_SGETPRINVT);
		return VirtualTerminal(id);
	}
	static VirtualTerminal getOut() {
		u32int id = RessourceCaller::sCall(VTIF_OBJTYPE, VTIF_SGETPROUTVT);
		return VirtualTerminal(id);
	}
	VirtualTerminal(u32int id) : RessourceCaller(id, VTIF_OBJTYPE) {}

	void writeHex(u32int number) {
		doCall(VTIF_WRITEHEX, number);
	}
	void writeDec(s64int number) {
		doCall(VTIF_WRITEDEC, (number >> 32), number);
	}
	void write(String s) {
		doCall(VTIF_WRITE, (u32int)&s);
	}
	String readLine() {
		return String::unserialize(doCall(VTIF_READLINE));
	}
	void setColor(u8int fg, u8int bg = 0xFF) {
		doCall(VTIF_SETCOLOR, (fg << 8) | bg);
	}
	void setCsrLine(u32int line) {
		doCall(VTIF_SETCSRLINE, line);
	}
	void setCsrCol(u32int col) {
		doCall(VTIF_SETCSRCOL, col);
	}
	bool isBoxed() {
		return doCall(VTIF_ISBOXED) != 0;
	}
	void put(WChar c) {
		doCall(VTIF_PUT, c);
	}
	
	inline VirtualTerminal& operator<<(const String& s) { write(s); return *this; }
	inline VirtualTerminal& operator<<(s32int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(s64int i) { writeDec(i); return *this; }
	inline VirtualTerminal& operator<<(u32int i) { writeHex(i); return *this; }
};

extern VirtualTerminal invt, outvt;
