#include "WChar.class.h"

#ifdef THIS_IS_MELON_KERNEL
using namespace CMem;
#endif

#ifdef THIS_IS_MELON_USERLAND
using namespace CMem;
#endif

WChar WChar::CP437[] = {	//These are the UTF8 equivalents for the 128 extra characters of code page 437
	"Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
	"É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "¢", "£", "¥", "₧", "ƒ",
	"á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "⌐", "¬", "½", "¼", "¡", "«", "»",
	"░", "▒", "▓", "│", "┤", "╡", "╢", "╖", "╕", "╣", "║", "╗", "╝", "╜", "╛", "┐",
	"└", "┴", "┬", "├", "─", "┼", "╞", "╟", "╚", "╔", "╩", "╦", "╠", "═", "╬", "¤",
	"╨", "╤", "╥", "╙", "╘", "╒", "╓", "╫", "╪", "┘", "┌", "█", "▄", "▌", "▐", "▀",
	"α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε", "∩",
	"≡", "±", "≥", "≤", "⌠", "⌡", "÷", "≈", "°", "∙", "·", "√", "ⁿ", "²", "■", "⍽"
};

WChar::WChar() {
	value = 0;
}

WChar::WChar(char c) {
	affectAscii(c);
}

WChar::WChar(const char* c, u8int encoding) {
	if (encoding == UE_UTF8) 	affectUtf8(c);
	if (encoding == UE_UTF16_LE)	affectUtf16le(c);
	if (encoding == UE_UTF16_BE)	affectUtf16be(c);
	if (encoding == UE_UTF32_LE)	affectUtf32le(c);
	if (encoding == UE_UTF32_BE)	affectUtf32be(c);
}

u32int WChar::ucharLen(const char* c, u8int encoding) {
	if (encoding == UE_UTF8) {
		if ((c[0] & 0x80) == 0) return 1;
		else if ((c[0] & 0xE0) == 0xC0) return 2;
		else if ((c[0] & 0xF0) == 0xE0) return 3;
		else if ((c[0] & 0xF8) == 0xF0) return 4;
		else return 1;
	} else if (encoding == UE_UTF16_BE) {
		if ((c[0] & 0xFC) == 0xD8 and (c[2] & 0xFC) == 0xDC) return 4;
		else return 2;
	} else if (encoding == UE_UTF16_LE) {
		if ((c[1] & 0xFC) == 0xD8 and (c[3] & 0xFC) == 0xDC) return 4;
		else return 2;
	} else if (encoding == UE_UTF32_LE or encoding == UE_UTF16_BE) {
		return 4;
	}
	return 1;
}

u32int WChar::utfLen(const char* c, u8int encoding) {
	int i = 0, co = 0;
	while (WChar(c + i, encoding) != 0) {
		i += ucharLen(c + i, encoding);
		co++;
	}
	return co;
}

void WChar::affectAscii(char c) {
	s32int a = c;
	if (a >= 0)	value = a;
	else value = CP437[a + 128];
}

u32int WChar::affectUtf8(const char* c) {	//Returns the number of bytes for the character
	if ((c[0] & 0x80) == 0) {
		value = c[0];		//0x80 = 10000000b
		return 1;
	}
	if ((c[0] & 0xE0) == 0xC0) {				// 11100000b, 11000000b
		value = ((c[0] & 0x1F) << 6) | (c[1] & 0x3F);
		if (value < 128) value = 0;	//Bad value
		return 2;
	}
	if ((c[0] & 0xF0) == 0xE0) {	// 11110000b, 11100000b
		value = ((c[0] & 0x0F) << 12) | ((c[1] & 0x3F) << 6) | (c[2] & 0x3F);
		if (value < 2048) value = 0; //Bad value	
		if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
		if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
		return 3;
	}
	if ((c[0] & 0xF8) == 0xF0) {	// 11111000b, 11110000b
		value = ((c[0] & 0x0E) << 18) | ((c[1] & 0x3F) << 12) | ((c[2] & 0x3F) << 6) | (c[3] & 0x3F);
		if (value < 65536) value = 0; //Bad value
		return 4;
	}
	value = 0;	//Something wrong happenned
	return 1;
}

u32int WChar::affectUtf16be(const char* c) {
	if ((c[0] & 0xFC) == 0xD8 and		// 11111100b, 11011000b
		(c[2] & 0xFC) == 0xDC) {		// 11111100b, 11011100b
		u32int w = ((c[0] & 0x03) << 2) | ((c[1] & 0xC0) >> 6);
		u32int x = (c[1] & 0x3F);
		u32int y = ((c[2] & 0x03) << 8) | (c[2]);
		value = ((w + 1) << 16) | (x << 10) | y;
		if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
		if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
		return 4;
	} else {
		value = (c[0] << 8) | (c[1]);
		if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
		if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
		return 2;
	}
}

u32int WChar::affectUtf16le(const char* c) {
	if ((c[1] & 0xFC) == 0xD8 and		// 11111100b, 11011000b
		(c[3] & 0xFC) == 0xDC) {		// 11111100b, 11011100b
		u32int w = ((c[1] & 0x03) << 2) | ((c[0] & 0xC0) >> 6);
		u32int x = (c[0] & 0x3F);
		u32int y = ((c[3] & 0x03) << 8) | (c[3]);
		value = ((w + 1) << 16) | (x << 10) | y;
		if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
		if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
		return 4;
	} else {
		value = (c[1] << 8) | (c[0]);
		if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
		if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
		return 2;
	}
}

u32int WChar::affectUtf32be(const char* c) {
	value = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
	if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
	if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
	return 4;
}

u32int WChar::affectUtf32le(const char* c) {
	value = (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
	if (value >= 0xD800 and value <= 0xDFFF) value = 0;	//These values are unallowed
	if (value >= 0xFFFE and value <= 0xFFFF) value = 0;	
	return 4;
}

u8int WChar::toAscii() {
	if (value < 128) return (char)value;
	for (int i = 0; i < 128; i++) {
		if (CP437[i] == value) return (i + 128);
	}
	return '?';
}

uchar_repr_t WChar::toUtf8() {
	uchar_repr_t r;
	r.i = 0;
	if (value < 128) {
		r.c[0] = value;
	} else if (value < 4096) {
		r.c[0] = 0xC0 | ((value & 0x07C0) >> 6);
		r.c[1] = 0x80 | (value & 0x3F);
	} else if (value < 65536) {
		r.c[0] = 0xE0 | ((value & 0xF000) >> 12);
		r.c[1] = 0x80 | ((value & 0x0FC0) >> 6);
		r.c[2] = 0x80 | (value & 0x003F);
	} else {
		r.c[0] = 0xF0 | ((value & 0x1C0000) >> 18);
		r.c[1] = 0x80 | ((value & 0x3F000) >> 12);
		r.c[2] = 0x80 | ((value & 0x0FC0) >> 6);
		r.c[3] = 0x80 | (value & 0x003F);
	}
	return r;
}

//TODO : code WChar::toUtf16(be|le)

uchar_repr_t WChar::toUtf32be() {
	uchar_repr_t r;
	r.c[0] = (value >> 24) & 0xFF;
	r.c[1] = (value >> 16) & 0xFF;
	r.c[2] = (value >> 8) & 0xFF;
	r.c[3] = value & 0xFF;
	return r;
}

uchar_repr_t WChar::toUtf32le() {
	uchar_repr_t r;
	r.c[3] = (value >> 24) & 0xFF;
	r.c[2] = (value >> 16) & 0xFF;
	r.c[1] = (value >> 8) & 0xFF;
	r.c[0] = value & 0xFF;
	return r;
}
