#include "StringStream.class.h"

StringIStream::StringIStream(const String &e) {
	m_elements = new SimpleList<String>(e);
}

StringIStream::StringIStream() {
	m_elements = 0;
}

StringIStream::~StringIStream() {
	if (m_elements != 0) delete m_elements;
}

String StringIStream::read() {
	if (m_elements == 0) return "";
	String ret = m_elements->v();
	m_elements = m_elements->delThis();
	return ret;
}

void StringIStream::append(const String &e) {
	if (m_elements == 0) {
		m_elements = new SimpleList<String>(e);
	} else {
		m_elements->addAtEnd(e);
	}
}
