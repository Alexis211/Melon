#ifndef DEF_BYTEARRAY_CLASS_H
#define DEF_BYTEARRAY_CLASS_H

#include <String.class.h>

class ByteArray : public BasicString<u8int> {
	public:
	ByteArray() : BasicString<u8int>() {}
	ByteArray(const BasicString<u8int> &bs) : BasicString<u8int>() {
		m_length = bs.size();
		m_string = new u8int[m_length];
		for (u32int i = 0; i < m_length; i++)
			m_string[i] = bs[i];
	}
	ByteArray(const ByteArray& other) : BasicString<u8int>(other) {}
	ByteArray(const char* c, u32int len) : BasicString<u8int>((u8int*)c, len) {}
	ByteArray(const char* c);
	ByteArray(u32int size) : BasicString<u8int>((u8int)0, size) {}
	ByteArray(const String &string, u8int encoding = UE_UTF8) : BasicString<u8int>() { affect(string, encoding); }

	void affect(const String& string, u8int encoding = UE_UTF8);
	void resize(u32int size);

	String toString(u8int encoding = UE_UTF8);
	operator u8int* () { return m_string; }
};

#endif
