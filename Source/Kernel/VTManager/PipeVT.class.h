#ifndef DEF_PIPEVT_CLASS_H
#define DEF_PIPEVT_CLASS_H

#include <VTManager/VirtualTerminal.proto.h>

class PipeVT : public VirtualTerminal {
	protected:
	u32int m_col;

	public:
	PipeVT();

	bool isBoxed() { return false; }
	void setCursorCol(u32int col);
	void put(WChar c, bool updatecsr = true);
};

#endif
