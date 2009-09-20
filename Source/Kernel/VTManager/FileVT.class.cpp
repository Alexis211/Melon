#include "FileVT.class.h"

FileVT::FileVT(String filename, u8int mode, FSNode *start, u8int encoding) :
		m_file(filename, mode, start, encoding), m_buffer() {
	if (mode != FM_READ) {
		m_isWriting = true;
		m_bufferPos = 0;
	} else {
	   	m_isWriting = false;
		m_buffer = m_file.readLine();
	}
}
	
void FileVT::setCursorCol(u32int col) {
	if (!m_isWriting) return;
	while (m_buffer.size() < col) {
		put(" ");
	}
}

void FileVT::put(WChar c, bool updatecsr) {
	if (!m_isWriting) return;
	if (c.value == '\n') {
		m_file.write(m_buffer, true);
		m_buffer.clear();
	} else if (c.value == '\t') {
		u32int nc = (m_buffer.size() + 8) &~(8 - 1);
		while (m_buffer.size() < nc) put(" ");
	} else {
		m_buffer += c;
	}
}

Kbd::keypress_t FileVT::getKeypress(bool show, bool block) {
	Kbd::keypress_t ret;
	if (m_isWriting) return ret;

	if (m_bufferPos == m_buffer.size()) {
		ret.hascmd = true;
		ret.command = KBDC_ENTER;
		if (m_file.eof()) {
			m_buffer = ".";
		} else {
			m_buffer = m_file.readLine();
		}
		m_bufferPos = 0;
	} else {
		WChar c = m_buffer[m_bufferPos];
		m_bufferPos++;
		if (c.value == '\t') {
			ret.hascmd = true;
			ret.command = KBDC_TAB;
		} else if (c.value == '\b') {
			ret.hascmd = true;
			ret.command = KBDC_BACKSPACE;
		} else {
			ret.haschar = true;
			ret.character = c;
		}
	}	
	return ret;
}
