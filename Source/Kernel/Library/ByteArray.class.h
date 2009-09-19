#ifndef DEF_BYTEARRAY_CLASS_H
#define DEF_BYTEARRAY_CLASS_H

#include <Library/String.class.h>
#include <VTManager/VirtualTerminal.class.h>

class ByteArray : public BasicString<u8int> {
	public:
	ByteArray() : BasicString<u8int>() {}
	ByteArray(const ByteArray& other) : BasicString<u8int>(other) {}
	ByteArray(const char* c);
	ByteArray(u32int size) : BasicString<u8int>((u8int)0, size) {}
	ByteArray(const String &string, u8int encoding = UE_UTF8) : BasicString<u8int>() { affect(string, encoding); }

	void affect(const String& string, u8int encoding = UE_UTF8);
	void resize(u32int size);

	void dump(VirtualTerminal *vt);

	operator String ();
	operator u8int* () { return m_string; }
};

#endif
