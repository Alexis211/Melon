#include "ByteArray.class.h"

//Define size of a uchar_repr_t
#define CHARSZ(x) (x.c[0] == 0 ? 0 : (x.c[1] == 0 ? 1 : (x.c[2] == 0 ? 2 : (x.c[3] == 0 ? 3 : 4))))

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
	char* c = new char[m_length + 1];
	memcpy((u8int*)c, m_string, m_length);
	c[m_length] = 0;	//Add NULL terminator
	String r(c, encoding);
	delete c;
	return r;
}
