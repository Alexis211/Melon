#ifndef DEF_UCHAR_CLASS_H
#define DEF_UCHAR_CLASS_H

#include <Core/common.wtf.h>

struct wchar {
	u32int value;
	static wchar CP437[];

	wchar();		//Creates a null character
	wchar(char c);	//From ascii character
	wchar(char* c);	//From utf8 string

	static u32int utf8len(char* c);	//Returns count of utf8 characters in string

	void affectAscii(char c);
	u32int affectUtf8(char* c);
	void affectUtf16(char* c);
	void affectUtf32(char* c);
	u8int toAscii();

	inline wchar operator+ (u32int other) {
		wchar r;
		r.value = value + other;
		return r;
	}
	inline wchar operator- (u32int other) {
		wchar r;
		r.value = value - other;
		return r;
	}
	inline wchar& operator+= (u32int other) {
		value += other;
		return *this;
	}
	inline wchar& operator-= (u32int other) {
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
