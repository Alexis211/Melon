#include "PipeVT.class.h"
#include <TaskManager/Task.ns.h>

PipeVT::PipeVT() {
	m_col = 0;
}

void PipeVT::handleEscape(mvt_esc_cmd_t cmd) {
	if (cmd.cmd == MVTCMD_SETCSRCOL) {
		while (cmd.a > (int)m_col) {
			put(" ");
		}
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
	while (m_kbdbuffEnd == m_kbdbuffStart - 1 or (m_kbdbuffEnd == KBDBUFFSIZE - 1 and m_kbdbuffStart == 0)) 
		Task::currThread()->sleep(10);
	m_kbdbuff[m_kbdbuffEnd] = kp;
	m_kbdbuffEnd++;
	if (m_kbdbuffEnd == KBDBUFFSIZE) m_kbdbuffEnd = 0;
}
