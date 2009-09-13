#include "String.class.h"
#include <Library/Vector.class.h>

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

String::String() {
	m_string = 0;
	m_length = 0;
}

String::String(const char* string) {
	m_length = WChar::utf8len(string);
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new WChar[m_length + 1];
	int i = 0, l = strlen(string), c = 0;
	while (i < l) {
		i += m_string[c].affectUtf8(string + i);
		c++;
	}
	m_string[m_length] = 0;
}

String::String(const String &other) {
	m_length = other.m_length;
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new WChar[m_length + 1];
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = other.m_string[i];
	}
	m_string[m_length] = 0;
}

String::~String() {
	if (m_string != 0) delete [] m_string;
}

void String::operator= (const String &other) {
	m_length = other.m_length;
	if (m_string != 0) delete [] m_string;
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new WChar[m_length + 1];
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = other.m_string[i];
	}
	m_string[m_length] = 0;
}

void String::operator= (const char* string) {
	m_length = WChar::utf8len(string);
	if (m_string != 0) delete [] m_string;
	if (m_length == 0) {
		m_string = 0;
		return;
	}
	m_string = new WChar[m_length + 1];
	int i = 0, l = strlen(string), c = 0;
	while (i < l) {
		i += m_string[c].affectUtf8(string + i);
		c++;
	}
	m_string[m_length] = 0;
}

bool String::operator== (const String &other) {
	if (m_length != other.m_length) return false;
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] != other.m_string[i]) return false;
	}
	return true;
}

bool String::operator== (const char* string) {
	if (m_length != WChar::utf8len(string)) return false;
	int i = 0, l = strlen(string), c = 0;
	WChar tmp;
	while (i < l) {
		i += tmp.affectUtf8(string + i);
		if (m_string[c] != tmp) return false;
		c++;
	}
	return true;
}

String& String::operator+= (String &other) {
	WChar* newdata = new WChar[m_length + other.m_length + 1];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	for (u32int i = 0; i < other.m_length; i++) {
		newdata[i + m_length] = other.m_string[i];
	}
	if (m_string != 0) delete [] m_string;
	m_string = newdata;
	m_length += other.m_length;
	m_string[m_length] = 0;
	return *this;
}

String& String::operator+= (const char* other) {
	WChar* newdata = new WChar[m_length + WChar::utf8len(other) + 1];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	int i = 0, l = strlen(other), c = 0;
	while (i < l) {
		i += newdata[c + m_length].affectUtf8(other + i);
		c++;
	}
	if (m_string != 0) delete [] m_string;
	m_string = newdata;
	m_length += strlen(other);
	m_string[m_length] = 0;
	return *this;
}

String& String::operator+= (WChar other) {
	WChar* newdata = new WChar[m_length + 2];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	if (m_string != 0) delete[] m_string;
	m_string = newdata;
	m_string[m_length] = other;
	m_length++;
	m_string[m_length].value = 0;
	return *this;
}

String& String::operator+ (String &other) {	//Can be optimized
	String ret(*this);
	return (ret += other);
}

String& String::operator+ (const char* other) { //Can be optimized
	String ret(*this);
	return (ret += other);
}

String& String::operator+ (WChar other) {
	String ret(*this);
	return (ret += other);
}

s32int String::toInt() {
	if (m_string == 0) return 0;
	s32int pos = 0, number = 0;
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

u32int String::toInt16() {
	if (m_string == 0) return 0;
	u32int pos = 0, number = 0;
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

WChar& String::operator[] (int index) {
	return m_string[index];
}

u32int String::size() {
	return m_length;
}

void String::clear() {
	delete [] m_string;
	m_length = 0;
	m_string = 0;
}

bool String::empty() {
	return (m_length == 0);
}

Vector<String> String::split(WChar c) {
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

String String::substr(s32int start, s32int size) {
	if (start < 0) start = m_length - start;
	if (size < 0) { //this fucks
		start = start + size;
		size = 0 - size;
	}
	String ret;
	ret.m_string = new WChar[size + 1];
	ret.m_length = size;
	memcpy((u8int*)ret.m_string, (const u8int*)(m_string + start), size * sizeof(WChar));
	ret.m_string[size] = 0;
	return ret;
}
