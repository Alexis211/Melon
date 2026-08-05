#ifndef DEF_WCHAR_CLASS_H
#define DEF_WCHAR_CLASS_H

#include <types.h>
#include <Vector.class.h>

class WChar {
public:
	u32int value;

	WChar() : value(0) {}
	WChar(u32int v) : value(v) {}
	WChar(u16int v) : value(v) {}
	WChar(char c) : value(c) {}
	WChar(const char* c) : value(c ? *c : 0) {}

	bool operator<(const WChar& other) const { return value < other.value; }
	bool operator>(const WChar& other) const { return value > other.value; }
	bool operator<=(const WChar& other) const { return value <= other.value; }
	bool operator>=(const WChar& other) const { return value >= other.value; }
	bool operator==(const WChar& other) const { return value == other.value; }
	bool operator!=(const WChar& other) const { return value != other.value; }

	WChar& operator+=(const WChar& other) { value += other.value; return *this; }
	WChar& operator-=(const WChar& other) { value -= other.value; return *this; }
	WChar operator+(const WChar& other) const { return WChar(value + other.value); }
	WChar operator-(const WChar& other) const { return WChar(value - other.value); }

	Vector<u16int> toUtf16be() const;
	Vector<u16int> toUtf16le() const;
};

#endif
