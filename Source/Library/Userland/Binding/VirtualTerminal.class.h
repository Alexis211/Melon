#ifndef DEF_VIRTUALTERMINAL_CLASS_H
#define DEF_VIRTUALTERMINAL_CLASS_H

#include <Syscall/RessourceCaller.class.h>

#include <VirtualTerminal.iface.h>
#include <Kbd.iface.h>

#include <OStream.proto.h>
#include <IStream.proto.h>
#include <WChar.class.h>

class VirtualTerminal : public RessourceCaller, public OStream, public IStream {
	private:
	bool m_eof;

	public:
	static VirtualTerminal getIn() {
		u32int id = RessourceCaller::sCall(VTIF_OBJTYPE, VTIF_SGETPRINVT);
		return VirtualTerminal(id);
	}
	static VirtualTerminal getOut() {
		u32int id = RessourceCaller::sCall(VTIF_OBJTYPE, VTIF_SGETPROUTVT);
		return VirtualTerminal(id);
	}
	VirtualTerminal(u32int id) : RessourceCaller(id, VTIF_OBJTYPE) { m_eof = false; }

	//******** SYSCALLS ********
	void write(const String &s) {
		doCall(VTIF_WRITE, (u32int)&s);
	}
	void put(WChar c) {
		doCall(VTIF_PUT, c);
	}
	String read() {
		if (m_eof) return "";
		String ret = String::unserialize(doCall(VTIF_READLINE, 1));
		if (ret[ret.size() - 1] == WChar(EOF)) {
			ret = ret.substr(0, ret.size() - 1);
			if (ret.empty()) return "";
			m_eof = true;
		}
		return ret += "\n";
	}
	String readLine(bool show = true) {
		flush();
		return String::unserialize(doCall(VTIF_READLINE, (show ? 1 : 0)));
	}
	keypress_t getKeypress(bool show = true, bool block = true) {
		keypress_t* ptr = (keypress_t*)doCall(VTIF_GETKEYPRESS, (show ? 1 : 0) | (block ? 2 : 0));
		return *ptr;
	}
	//********* GET VT INFO SYSCALLS ********
	bool isBoxed() {
		return doCall(VTIF_ISBOXED) != 0;
	}
	u8int height() {
		return doCall(VTIF_GETHEIGHT);
	}
	u8int width() {
		return doCall(VTIF_GETWIDTH);
	}
	//********** HANDLE COMMAND *******
	VirtualTerminal& operator << (const String& s) { OStream::operator<<(s); return *this; }
	VirtualTerminal& operator << (s64int i) { OStream::operator<<(i); return *this; }
	VirtualTerminal& operator << (s32int i) { OStream::operator<<(i); return *this; }
	VirtualTerminal& operator << (u32int i) { OStream::operator<<(i); return *this; }
	VirtualTerminal& operator << (ostream_modifiers_e m) { OStream::operator<<(m); return *this; }
	VirtualTerminal& operator << (mvt_esc_cmd_t cmd) {
		String s("    ");
		s[0] = 0x1A;
		s[1] = cmd.cmd;
		s[2] = cmd.a;
		s[3] = cmd.b;
		OStream::put(s);
		return *this;
	}
};

#endif
