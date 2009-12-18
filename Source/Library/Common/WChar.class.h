#ifndef DEF_UCHAR_CLASS_H
#define DEF_UCHAR_CLASS_H

#include <types.h>

#ifndef THIS_IS_NOT_MELON
#include <common.h>
#endif

enum {
	UE_UTF8,
	UE_UTF16_LE,
	UE_UTF16_BE,
	UE_UTF32_LE,
	UE_UTF32_BE,
};

union uchar_repr_t {
	char c[4];
	u32int i;
};

//Define size of a uchar_repr_t
#define CHARSZ(x) (x.c[0] == 0 ? 0 : (x.c[1] == 0 ? 1 : (x.c[2] == 0 ? 2 : (x.c[3] == 0 ? 3 : 4))))

struct WChar {
	u32int value;
	static WChar CP437[];		//Codepage 437, used for conversion from/to ascii

	WChar();		//Creates a null character
	WChar(char c);	//From ascii character
	WChar(const char* c, u8int encoding = UE_UTF8);	//From utf8 string

	static u32int ucharLen(const char* c, u8int encoding = UE_UTF8);	//Returns count of bytes in one unicode character
	static u32int utfLen(const char* c, u8int encoding = UE_UTF8);	//Returns count of utf8 characters in string

	void affectAscii(char c);
	u32int affectUtf8(const char* c);
	u32int affectUtf16le(const char* c);
	u32int affectUtf16be(const char* c);
	u32int affectUtf32le(const char* c);
	u32int affectUtf32be(const char* c);

	u32int affect(const char* c, u8int encoding = UE_UTF8) {
		if (encoding == UE_UTF8) return affectUtf8(c);
		if (encoding == UE_UTF16_LE) return affectUtf16le(c);
		if (encoding == UE_UTF16_BE) return affectUtf16be(c);
		if (encoding == UE_UTF32_LE) return affectUtf32le(c);
		if (encoding == UE_UTF32_BE) return affectUtf32be(c);
		affectAscii(c[0]);	//Default case :/
		return 1;
	}

	u8int toAscii();

	uchar_repr_t toUtf8();
	uchar_repr_t toUtf16le();
	uchar_repr_t toUtf16be();
	uchar_repr_t toUtf32le();
	uchar_repr_t toUtf32be();

	uchar_repr_t encode(u8int encoding = UE_UTF8) {
		if (encoding == UE_UTF8) return toUtf8();
		//if (encoding == UE_UTF16_LE) return toUtf16le();
		//if (encoding == UE_UTF16_BE) return toUtf16be();
		if (encoding == UE_UTF32_LE) return toUtf32le();
		if (encoding == UE_UTF32_BE) return toUtf32be();
		uchar_repr_t x;
		x.c[0] = toAscii();
		return x;
	}

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

	inline operator u32int () const { return value; }
};

#endif
