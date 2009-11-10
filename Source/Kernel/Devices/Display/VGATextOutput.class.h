#ifndef DEF_VGATEXTOUTPUT_CLASS_H
#define DEF_VGATEXTOUTPUT_CLASS_H

#include <Devices/Display/Display.proto.h>

class VGATextOutput : public Display {
	int m_cols;

	public:
	VGATextOutput() : m_cols(80) {}

	String getClass();
	String getName();

	void getModes(Vector<Disp::mode_t> &to);
	bool setMode(Disp::mode_t& mode);

	void putChar(u16int line, u16int col, WChar c, u8int color);
	void moveCursor(u16int line, u16int col);
	void clear();
};

#endif
