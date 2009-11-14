#include "VirtualTerminal.proto.h"
#include <TaskManager/Task.ns.h>

using namespace Kbd;

void VirtualTerminal::keyPress(keypress_t kp) {
	if (m_kbdbuffEnd == m_kbdbuffStart - 1 or (m_kbdbuffEnd == KBDBUFFSIZE - 1 and m_kbdbuffStart == 0)) return;
	m_kbdbuff[m_kbdbuffEnd] = kp;
	m_kbdbuffEnd++;
	if (m_kbdbuffEnd == KBDBUFFSIZE) m_kbdbuffEnd = 0;
	if (!m_kbdMutex.locked()) {
		if (kp.haschar && !kp.hascmd) {
			put(kp.character);
		} else if (kp.hascmd && !kp.haschar && kp.command == KBDC_ENTER) {
			put("\n");
		} else if (kp.hascmd && !kp.haschar && kp.command == KBDC_TAB) {
			put("\t");
		} else if (kp.hascmd && !kp.haschar && kp.command == KBDC_BACKSPACE) {
			put("\b");
		}
	}
}

keypress_t VirtualTerminal::getKeypress(bool show, bool block) {
	m_kbdMutex.waitLock();

	if (m_kbdbuffStart == m_kbdbuffEnd && !block) {
		m_kbdMutex.unlock();
		return keypress_t();
	}

	while (m_kbdbuffStart == m_kbdbuffEnd) {
		Task::currThread()->sleep(10);
	}

	keypress_t ret = m_kbdbuff[m_kbdbuffStart];

	m_kbdbuffStart++;
	if (m_kbdbuffStart == KBDBUFFSIZE) m_kbdbuffStart = 0;

	m_kbdMutex.unlock();

	if (show) {
		if (ret.haschar && !ret.hascmd) {
			put(ret.character);
		} else if (ret.hascmd && !ret.haschar && ret.command == KBDC_ENTER) {
			put("\n");
		} else if (ret.hascmd && !ret.haschar && ret.command == KBDC_TAB) {
			put("\t");
		} else if (ret.hascmd && !ret.haschar && ret.command == KBDC_BACKSPACE) {
			put("\b");
		}
	}

	return ret;
}

String VirtualTerminal::readLine(bool show) {
	String ret = "";
	keypress_t tmp = getKeypress(show);
	while (!(tmp.hascmd && !tmp.haschar && tmp.command == KBDC_ENTER)) {
		if (tmp.hascmd && !tmp.haschar && tmp.command == KBDC_BACKSPACE) {
			if (!ret.empty()) ret = ret.substr(0, ret.size() - 1);
			else if (show) put(" ");	//Put a space so that cursor stays at same place
		} else if (tmp.hascmd && !tmp.haschar && tmp.command == KBDC_TAB) {
			ret += "\t";
		} else if (tmp.haschar && !tmp.hascmd) {
			ret += tmp.character;
		}
		tmp = getKeypress(show);
	}
	if (!show) put("\n");	//Put a return if it hasn't been shown on getChar();
	return ret;
}
