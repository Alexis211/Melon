#include "IStream.proto.h"

IStream::IStream() {
	m_buffer = NULL;
	m_ptr = 0;
}

IStream::IStream(const IStream& other) {
	m_buffer = NULL;
	m_ptr = 0;
}

IStream::~IStream() {
	delete m_buffer;
}

bool IStream::populate() {
	String s = read();
	if (s.empty()) return false;
	if (m_buffer == NULL) {
		m_buffer = new SimpleList<String>(s);
	} else {
		m_buffer->addAtEnd(s);
	}
	return true;
}

WChar IStream::getChar() {
	waitLock();
	if (m_buffer == 0) {
		if (!populate()) {
			unlock();
		   	return WChar("\0");
		}
	}
	WChar ret = m_buffer->v()[m_ptr];
	m_ptr++;
	if (m_ptr >= m_buffer->v().size()) {
		m_buffer = m_buffer->delThis();
		m_ptr = 0;
	}
	unlock();
	return ret;
}

String IStream::get(WChar delimiter) {
	waitLock();
	//calculate length of string to read
	if (m_buffer == 0) populate();
	int length = 0, ptr = m_ptr;
	for (SimpleList<String> *iter = m_buffer; iter != 0;) {
		if (iter->v()[ptr] == delimiter) break;
		ptr++;
		length++;
		if (ptr >= iter->v().size()) {
			if (iter->next() == 0) populate();
			iter = iter->next();
			ptr = 0;
		}
	}
	//get it
	String ret(WChar(" "), length);
	for (int i = 0; i <= length; i++) {
		if (i != length) ret[i] = m_buffer->v()[m_ptr];
		m_ptr++;
		if (m_ptr >= m_buffer->v().size()) {
			m_buffer = m_buffer->delThis();
			m_ptr = 0;
		}
	}
	unlock();
	return ret;
}
