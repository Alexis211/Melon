#include <Library/Vector.class.h>

#define FREE if (m_string != 0) delete m_string;
#define ALLOC m_string = new T[m_length];
#define VRFY if (m_length == 0) { m_string = NULL; return; }

using namespace CMem;

template <typename T>
BasicString<T>::BasicString() {
	m_string = NULL;
	m_length = 0;
}

template <typename T>
BasicString<T>::BasicString(const T* string, u32int length) {
	m_string = NULL;
	affect(string, length);
}

template <typename T>
BasicString<T>::BasicString(const BasicString<T> &other) {
	m_string = NULL;
	affect(other);
}

template <typename T>
BasicString<T>::BasicString(const T value, u32int count) {
	m_string = NULL;
	affect(value, count);
}

template <typename T>
BasicString<T>::~BasicString() {
	FREE;
}

template <typename T>
void BasicString<T>::affect(const BasicString<T> &other) {
	FREE;
	m_length = other.m_length;
	VRFY;
	ALLOC;
	memcpy((u8int*)m_string, (u8int*)(other.m_string), m_length * sizeof(T));
}

template <typename T>
void BasicString<T>::affect(const T* string, u32int length) {
	FREE;
	m_length = length;
	VRFY;
	ALLOC;
	memcpy((u8int*)string, (u8int*)string, m_length * sizeof(T));
}

template <typename T>
void BasicString<T>::affect(const T value, u32int count) {
	FREE;
	m_length = count;
	VRFY;
	ALLOC;
	for (u32int i = 0; i < count; i++) {
		m_string[i] = value;
	}
}

template <typename T>
bool BasicString<T>::compare(const BasicString<T> &other) const {
	if (m_length != other.m_length) return false;
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] != other.m_string[i]) return false;
	}
	return true;
}

template <typename T>
bool BasicString<T>::compare(const T* string, u32int length) const {
	if (m_length != length) return false;
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] != string[i]) return false;
	}
	return true;
}

template <typename T>
BasicString<T> &BasicString<T>::append(const BasicString<T> &other) {
	T* newdata = new T[m_length + other.m_length];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	for (u32int i = 0; i < other.m_length; i++) {
		newdata[i + m_length] = other.m_string[i];
	}
	FREE;
	m_string = newdata;
	m_length += other.m_length;
	return *this;
}

template <typename T>
BasicString<T> &BasicString<T>::append(const T* string, u32int length) {
	T* newdata = new T[m_length + length];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	for (u32int i = 0; i < length; i++) {
		newdata[i + m_length] = string[i];
	}
	FREE;
	m_string = newdata;
	m_length += length;
	return *this;
}

template <typename T>
BasicString<T> &BasicString<T>::append(const T other) {
	T* newdata = new T[m_length + 1];
	for (u32int i = 0; i < m_length; i++) {
		newdata[i] = m_string[i];
	}
	FREE;
	m_string = newdata;
	m_string[m_length] = other;
	m_length++;
	return *this;
}

template <typename T>
BasicString<T> BasicString<T>::concat(const BasicString<T> &other) const {
	BasicString<T> ret(*this);
	return (ret.append(other));
}

template <typename T>
BasicString<T> BasicString<T>::concat(const T* string, u32int length) const {
	BasicString<T> ret(*this);
	return (ret.append(string, length));
}

template <typename T>
BasicString<T> BasicString<T>::concat(const T other) const {
	BasicString<T> ret(*this);
	return (ret.append(other));
}

template <typename T>
void BasicString<T>::clear() {
	FREE;
	m_string = 0;
	m_length = 0;
}

template <typename T>
Vector< BasicString<T> > BasicString<T>::split(T sep) const {
	Vector< BasicString<T> > ret;
	ret.push(BasicString<T>());
	for (u32int i = 0; i < m_length; i++) {
		if (m_string[i] == sep) {
			ret.push(BasicString<T>());
		} else {
			ret.back().append(m_string[i]);
		}
	}
	return ret;
}

template <typename T>
BasicString<T> BasicString<T>::substr(s32int start, u32int size) {
	if (start < 0) start = m_length - start;
	BasicString<T> ret;
	ret.m_string = new T[size + 1];
	ret.m_length = size;
	memcpy((u8int*)ret.m_string, (u8int*)(&m_string[start]), size * sizeof(T));
	return ret;
}
