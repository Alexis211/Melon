#ifndef DEF_SCROLLABLEVT_CLASS_H
#define DEF_SCROLLABLEVT_CLASS_H

#include <VTManager/SimpleVT.class.h>

class ScrollableVT : public SimpleVT {
	private:
	vtchr **m_lines;
	u32int m_keeprows;
	u32int m_linesup;

	public:
	ScrollableVT(u32int rows, u32int cols, u32int keepRows, u8int fgcolor = 7, u8int bgcolor = 0);
	virtual ~ScrollableVT();

	virtual void putChar(u32int row, u32int col, WChar c);
	void updateCursor();
	void redraw();
	void scroll();

	void keyPress(Kbd::keypress_t kp);
};

#endif
