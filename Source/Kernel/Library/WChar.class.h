#ifndef DEF_UCHAR_CLASS_H
#define DEF_UCHAR_CLASS_H

#include <Core/common.wtf.h>

enum {
	UE_UTF8,
	UE_UTF16,
	UE_UTF32,
};

union uchar_repr_t {
	char c[4];
	u32int i;
};

struct WChar {
	u32int value;
	static WChar CP437[];		//Codepage 437, used for conversion from/to ascii

	WChar();		//Creates a null character
	WChar(char c);	//From ascii character
	WChar(const char* c, u8int encoding = UE_UTF8);	//From utf8 string

	static u32int utfLen(const char* c, u8int encoding = UE_UTF8);	//Returns count of utf8 characters in string

	void affectAscii(char c);
	u32int affectUtf8(const char* c);
	u32int affectUtf16(const char* c);
	u32int affectUtf32(const char* c);

	u32int affect(const char* c, u8int encoding = UE_UTF8) {
		if (encoding == UE_UTF8) return affectUtf8(c);
		if (encoding == UE_UTF16) return affectUtf16(c);
		if (encoding == UE_UTF32) return affectUtf32(c);
		affectAscii(c[0]);	//Default case :/
		return 1;
	}

	u8int toAscii();

	uchar_repr_t toUtf8();
	uchar_repr_t toUtf16();
	uchar_repr_t toUtf32();

	inline WChar operator+ (u32int other) {
		WChar r;
		r.value = value + other;
		return r;
	}
	inline WChar operator- (u32int other) {
		WChar r;
		r.value = value - other;
		return r;
	}
	inline WChar& operator+= (u32int other) {
		value += other;
		return *this;
	}
	inline WChar& operator-= (u32int other) {
		value -= other;
		return *this;
	}
	inline bool operator== (u32int other) {
		return value == other;
	}
	inline u32int operator= (u32int v) {
		value = v;
		return v;
	}

	inline operator u32int () { return value; }
};

#endif
