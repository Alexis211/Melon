#include "PipeVT.class.h"

PipeVT::PipeVT() {
	m_col = 0;
}

void PipeVT::setCursorCol(u32int col) {
	while (col > m_col) {
		put(" ");
	}
}

void PipeVT::put(WChar c, bool updatecsr) {
	keypress_t kp;
	if (c.value == '\t') {
		m_col = (m_col + 8) &~(8 - 1);
		kp.hascmd = true;
		kp.command = KBDC_TAB;
	} else if (c.value == '\n' or c.value == '\r') {
		m_col = 0;
		kp.hascmd = true;
		kp.command = KBDC_ENTER;
	} else if (c.value == '\b') {
		kp.hascmd = true;
		kp.command = KBDC_BACKSPACE;
	} else {
		kp.hascmd = false;
		kp.haschar = true;
		kp.character = c;
		m_col++;
	}
	m_kbdbuffMutex.waitLock();
	m_kbdbuff.push(kp);
	m_kbdbuffMutex.unlock();
}
