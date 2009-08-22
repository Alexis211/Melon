#include "String.class.h"
#include <Library/Vector.class.h>

using namespace CMem;	//strlen and memcpy

String String::hex(u32int number) {
	String ret;
	ret.m_length = 10;
	ret.m_string = (char*)Mem::kalloc(11);
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
	ret.m_string = (char*)Mem::kalloc(order + 1);

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

String::String(char* string) {
	m_length = strlen(string);
	m_string = (char*)Mem::kalloc(m_length + 1);
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = string[i];	
	}
	m_string[m_length] = 0;
}

String::String(const String &other) {
	m_length = other.m_length;
	m_string = (char*)Mem::kalloc(m_length + 1);
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = other.m_string[i];
	}
	m_string[m_length] = 0;
}

String::~String() {
	if (m_string != 0) Mem::kfree(m_string);
}

void String::operator= (const String &other) {
	m_length = other.m_length;
	if (m_string != 0) Mem::kfree(m_string);
	m_string = (char*)Mem::kalloc(m_length + 1);
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = other.m_string[i];
	}
	m_string[m_length] = 0;
}

void String::operator= (char* string) {
	m_length = strlen(string);
	if (m_string != 0) Mem::kfree(m_string);
	m_string = (char*)Mem::kalloc(m_length + 1);
	for (u32int i = 0; i < m_length; i++) {
		m_string[i] = string[i];	
	}
	m_string[m_length] = 0;
}

bool String::operator== (String &other) {
	if (m_length != other.m_length) return false;
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] != other.m_string[i]) return false;
	}
	return true;
}

bool String::operator== (char* string) {
	if (m_length != strlen(string)) return false;
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] != string[i]) return false;
	}
	return true;
}

String& String::operator+= (String &other) {
	char* newdata = (char*)Mem::kalloc(m_length + other.m_length + 1);
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	for (u32int i = 0; i < other.m_length; i++) {
		newdata[i + m_length] = other.m_string[i];
	}
	if (m_string != 0) Mem::kfree(m_string);
	m_string = newdata;
	m_length += other.m_length;
	m_string[m_length] = 0;
	return *this;
}

String& String::operator+= (char* other) {
	char* newdata = (char*)Mem::kalloc(m_length + strlen(other) + 1);
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	for (u32int i = 0; i < strlen(other); i++) {
		newdata[i + m_length] = other[i];
	}
	if (m_string != 0) Mem::kfree(m_string);
	m_string = newdata;
	m_length += strlen(other);
	m_string[m_length] = 0;
	return *this;
}

String& String::operator+= (char other) {
	char* newdata = (char*)Mem::kalloc(m_length + 2);
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	if (m_string != 0) Mem::kfree(m_string);
	m_string = newdata;
	m_string[m_length] = other;
	m_length++;
	m_string[m_length] = 0;
	return *this;
}

String& String::operator+ (String &other) {	//Can be optimized
	String ret(*this);
	return (ret += other);
}

String& String::operator+ (char* other) { //Can be optimized
	String ret(*this);
	return (ret += other);
}

String& String::operator+ (char other) { //Can be optimized
	String ret(*this);
	return (ret += other);
}

String::operator char* () {
	if (m_string == 0) return "";
	return m_string;
}

s32int String::toInt() {
	if (m_string == 0) return 0;
	s32int pos = 0, number = 0;
	bool negative = false;
	if (m_string[0] == '-') {
		negative = true;
		pos = 1;
	}
	while (m_string[pos] >= '0' && m_string[pos] <= '9') {
		number *= 10;
		number += (m_string[pos] - '0');
		pos++;
	}
	if (negative) return 0 - number;
	return number;
}

u32int String::toInt16() {
	if (m_string == 0) return 0;
	u32int pos = 0, number = 0;
	if (m_string[0] == '0' && m_string[1] == 'x') pos = 2;
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

char& String::operator[] (int index) {
	return m_string[index];
}

u32int String::size() {
	return m_length;
}

void String::clear() {
	Mem::kfree(m_string);
	m_length = 0;
	m_string = 0;
}

bool String::empty() {
	return (m_length == 0);
}

Vector<String> String::split(char c) {
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
	ret.m_string = (char*)Mem::kalloc(size + 1);
	ret.m_length = size;
	memcpy((u8int*)ret.m_string, (const u8int*)(m_string + start), size);
	ret.m_string[size] = 0;
	return ret;
}
