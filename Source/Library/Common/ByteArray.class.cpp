#include "ByteArray.class.h"

using namespace CMem;

ByteArray::ByteArray(const char* c) : BasicString<u8int>() {
	m_length = strlen(c);
	memcpy(m_string, (u8int*)c, m_length);
}

void ByteArray::affect(const String &string, u8int encoding) {
	m_length = 0;
	for (u32int i = 0; i < string.size(); i++) {
		uchar_repr_t a = string[i].encode(encoding);
		m_length += CHARSZ(a);
	}
	if (m_string != 0) delete m_string;
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new u8int[m_length];
	u32int x = 0;
	for (u32int i = 0; i < string.size(); i++) {
		uchar_repr_t a = string[i].encode(encoding);
		memcpy(m_string + x, (u8int*)a.c, CHARSZ(a));
		x += CHARSZ(a);
	}
}

void ByteArray::resize(u32int size) {
	if (size == m_length) return;
	if (size == 0) {
		delete m_string;
		m_length = 0;
		m_string = 0;
	}
	u8int *nd = new u8int[size];
	if (size < m_length) {
		memcpy(nd, m_string, size);
	} else {
		memcpy(nd, m_string, m_length);
		memset(nd + m_length, 0, size - m_length);
	}
	delete m_string;
	m_string = nd;
	m_length = size;
}

String ByteArray::toString (u8int encoding) {
	char* c = new char[m_length + 4];
	memcpy((u8int*)c, m_string, m_length);
	for (int i = 0; i < 4; i++) c[m_length + i] = 0;	//Add NULL terminator
	String r(c, encoding);
	delete c;
	return r;
}
