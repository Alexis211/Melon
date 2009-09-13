#include "WChar.class.h"

WChar WChar::CP437[] = {	//These are the UTF8 equivalents for the 128 extra characters of code page 850
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

WChar::WChar(const char* c) {
	affectUtf8(c);
}

u32int WChar::utf8len(const char* c) {
	int i = 0, l = CMem::strlen(c), co = 0;
	while (i < l) {
		if ((c[i] & 0x80) == 0) i += 1;
		else if ((c[i] & 0xE0) == 0xC0) i += 2;
		else if ((c[i] & 0xF0) == 0xE0) i += 3;
		else if ((c[i] & 0xF8) == 0xF0) i += 4;
		else i += 1;
		co++;
	}
	return co;
}

void WChar::affectAscii(char c) {
	if (c >= 0)	value = c;
	else value = CP437[c + 128];
}

u32int WChar::affectUtf8(const char* c) {	//Returns the number of bytes for the character
	/*if ((c[0] & 0xB0) == 0x80) {	//11000000b == 10000000b, means we are IN a sequence
		value = 0;
		return 1;
	}*/
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

u8int WChar::toAscii() {
	if (value < 128) return (char)value;
	for (int i = 0; i < 128; i++) {
		if (CP437[i] == value) return (i + 128);
	}
	return '?';
}
