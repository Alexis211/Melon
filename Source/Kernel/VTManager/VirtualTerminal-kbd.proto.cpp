#include "VirtualTerminal.proto.h"
#include <TaskManager/Task.ns.h>

using namespace Kbd;

void VirtualTerminal::keyPress(keypress_t kp) {
	m_kbdbuffMutex.waitLock();
	m_kbdbuff.push(kp);
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
	m_kbdbuffMutex.unlock();
}

keypress_t VirtualTerminal::getKeypress(bool show, bool block) {
	m_kbdMutex.waitLock();

	if (m_kbdbuff.empty() && !block) {
		m_kbdMutex.unlock();
		return keypress_t();
	}

	while (m_kbdbuff.empty())
		Task::currentThread->sleep(10);

	m_kbdbuffMutex.waitLock();
	keypress_t ret = m_kbdbuff[0];

	for (u32int i = 1; i < m_kbdbuff.size(); i++) {
		m_kbdbuff[i - 1] = m_kbdbuff[i];
	}
	m_kbdbuff.pop();
	m_kbdbuffMutex.unlock();

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

	m_kbdMutex.unlock();
	return ret;
}

String VirtualTerminal::readLine(bool show) {
	String ret = "";
	keypress_t tmp = getKeypress(show);
	while (!(tmp.hascmd && !tmp.haschar && tmp.command == KBDC_ENTER)) {
		if (tmp.hascmd && !tmp.haschar && tmp.command == KBDC_BACKSPACE) {
			if (!ret.empty()) ret = ret.substr(0, ret.size() - 1);
			else put(" ");	//Put a space so that cursor stays at same place
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
