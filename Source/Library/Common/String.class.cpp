#include "String.class.h"
#include <Vector.class.h>

using namespace CMem;	//strlen and memcpy

String String::hex(u32int number) {
	String ret;
	ret.m_length = 10;
	ret.m_string = new WChar[11];
	ret.m_string[0] = '0';
	ret.m_string[1] = 'x';
	ret.m_string[10] = 0;

	char hexdigits[] = "0123456789ABCDEF";
	for (unsigned int j = 0; j < 8; j++) {
		ret.m_string[j + 2] = hexdigits[(number & 0xF0000000) >> 28];
		number = number << 4;
	}	
	return ret;
}

String String::number(s32int number) {
	if (number == 0) return String("0");
	bool negative = false;
	if (number < 0) {
		negative = true;
		number = 0 - number;
	}
	u32int order = 0, temp = number;
	char numbers[] = "0123456789";
	while (temp > 0) {
		order++;
		temp /= 10;
	}

	String ret;
	ret.m_length = order;
	ret.m_string = new WChar[order + 1];

	for (u32int i = order; i > 0; i--) {
		ret.m_string[i - 1] = numbers[number % 10];
		number /= 10;
	}

	ret.m_string[order] = 0;

	if (negative) return String("-") += ret;

	return ret;
}

String String::unserialize(u32int w) {
	if (w == (u32int) - 1) return String();
	u32int* a = (u32int*)w;
	String ret;
	ret.m_length = a[0];
	ret.m_string = (WChar*)Mem::alloc(a[0] * sizeof(WChar));
	for (u32int i = 0; i < a[0]; i++) {
		ret[i] = a[i + 1];
	}
	return ret;
}

u32int String::serialize() const {
	u32int* x = (u32int*)Mem::mkXchgSpace((m_length + 1) * sizeof(u32int));
	x[0] = m_length;
	for (u32int i = 0; i < m_length; i++) {
		x[i + 1] = m_string[i];
	}
	return (u32int)x;
}

String::String(const char* string, u8int encoding) {
	m_string = 0;
	m_length = 0;
	affect(string, encoding);
}

void String::affect (const char* string, u8int encoding) {
	m_length = WChar::utfLen(string, encoding);
	if (m_string != 0) delete [] m_string;
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new WChar[m_length + 1];
	u32int i = 0, c = 0;
	while (c < m_length) {
		i += m_string[c].affect(string + i, encoding);
		c++;
	}
	m_string[m_length] = 0;
}

bool String::compare (const char* string, u8int encoding) const {
	if (m_length != WChar::utfLen(string, encoding)) return false;
	int i = 0, l = strlen(string), c = 0;
	WChar tmp;
	while (i < l) {
		i += tmp.affect(string + i, encoding);
		if (m_string[c] != tmp) return false;
		c++;
	}
	return true;
}

String& String::append (const char* other, u8int encoding) {
	WChar* newdata = new WChar[m_length + WChar::utfLen(other, encoding) + 1];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	int i = 0, l = strlen(other), c = 0;
	while (i < l) {
		i += newdata[c + m_length].affect(other + i, encoding);
		c++;
	}
	if (m_string != 0) delete [] m_string;
	m_string = newdata;
	m_length += strlen(other);
	m_string[m_length] = 0;
	return *this;
}

String String::concat (const String &other) const {	//Can be optimized
	String ret(*this);
	return (ret += other);
}

String String::concat (const char* other, u8int encoding) const { //Can be optimized
	String ret(*this);
	return (ret.append(other, encoding));
}

String String::concat (WChar other) const {
	String ret(*this);
	return (ret += other);
}

s64int String::toInt() const {
	if (m_string == 0) return 0;
	s32int pos = 0;
	s64int number = 0;
	bool negative = false;
	if (m_string[0].value == '-') {
		negative = true;
		pos = 1;
	}
	while (m_string[pos] >= '0' && m_string[pos] <= '9') {
		number *= 10;
		number += (m_string[pos].value - '0');
		pos++;
	}
	if (negative) return 0 - number;
	return number;
}

u64int String::toInt16() const {
	if (m_string == 0) return 0;
	u32int pos = 0;
	u64int number = 0;
	if (m_string[0].value == '0' && m_string[1].value == 'x') pos = 2;
	while (1) {
		char c = m_string[pos];
		pos++;
		if (c >= 'a' && c <= 'f') c -= ('a' - 'A');		//To uppercase
		if (c >= '0' && c <= '9') {
			number *= 16;
			number += (c - '0');
			continue;
		}
		if (c >= 'A' && c <= 'F') {
			number *= 16;
			number += (c - 'A' + 10);
			continue;
		}
		break;
	}
	return number;
}

Vector<String> String::split(WChar c) const {
	Vector<String> ret;
	ret.push(String(""));
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] == c) {
			ret.push(String(""));
		} else {
			ret.back() += m_string[i];
		}
	}
	return ret;
}

String String::substr(s32int start, u32int size) {
	if (start < 0) start = m_length - start;
	if (size == 0) size = m_length - start;
	String ret;
	ret.m_string = new WChar[size + 1];
	ret.m_length = size;
	memcpy((u8int*)ret.m_string, (const u8int*)(m_string + start), size * sizeof(WChar));
	ret.m_string[size] = 0;
	return ret;
}
