#ifndef DEF_UCHAR_CLASS_H
#define DEF_UCHAR_CLASS_H

#include <Core/common.wtf.h>

struct WChar {
	u32int value;
	static WChar CP437[];

	WChar();		//Creates a null character
	WChar(char c);	//From ascii character
	WChar(const char* c);	//From utf8 string

	static u32int utf8len(const char* c);	//Returns count of utf8 characters in string

	void affectAscii(char c);
	u32int affectUtf8(const char* c);
	void affectUtf16(const char* c);
	void affectUtf32(const char* c);
	u8int toAscii();

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
