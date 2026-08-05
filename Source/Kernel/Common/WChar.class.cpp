#include "WChar.class.h"

Vector<u16int> WChar::toUtf16be() const {
	Vector<u16int> out;
	if (value <= 0xFFFF) {
		out.push(((value >> 8) & 0xFF) | ((value & 0xFF) << 8));
	} else {
		u32int cp = value - 0x10000;
		u16int high = 0xD800 | (cp >> 10);
		u16int low = 0xDC00 | (cp & 0x3FF);
		out.push(((high >> 8) & 0xFF) | ((high & 0xFF) << 8));
		out.push(((low >> 8) & 0xFF) | ((low & 0xFF) << 8));
	}
	return out;
}

Vector<u16int> WChar::toUtf16le() const {
	Vector<u16int> out;
	if (value <= 0xFFFF) {
		out.push((u16int)value);
	} else {
		u32int cp = value - 0x10000;
		u16int high = 0xD800 | (cp >> 10);
		u16int low = 0xDC00 | (cp & 0x3FF);
		out.push(high);
		out.push(low);
	}
	return out;
}
