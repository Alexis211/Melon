#include <OStream.proto.h>

OStream::OStream() {
	m_buffer = NULL;
	m_last = NULL;
}

OStream::OStream(const OStream& other) {
	m_buffer = NULL;
	m_last = NULL;
}

void OStream::put(const String &s) {
	waitLock();
	if (m_buffer == NULL or m_last == NULL) {
		m_buffer = m_last = new SimpleList<String>(s);
	} else {
		m_last = m_last->addAtEnd(s);
	}
	unlock();
}

void OStream::flush() {
	if (m_buffer == NULL or m_last == NULL) return;

	waitLock();
	//Calculate buffer size
	int size = 0;
	SimpleList<String>* iter = m_buffer;
	for (; iter != 0; iter = iter->next()) {
		size += iter->v().size();
	}
	//Get stuff
	String buff(WChar(" "), size);
	iter = m_buffer;
	for (int i = 0, pos = 0; i < size; i++, pos++) {
		if (pos >= iter->v().size()) {
			iter = iter->next();
			pos = 0;
		}
		buff[i] = iter->v()[pos];
	}
	//Write it
	write(buff);

	delete m_buffer;
	m_buffer = NULL;
	m_last = NULL;

	unlock();
}

OStream& OStream::operator<< (ostream_modifiers_e m) {
	if (m == FLUSH) {
		flush();
	} else if (m == ENDL) {
		put("\n");
	}
	return *this;
}
