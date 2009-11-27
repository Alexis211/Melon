#ifndef DEF_BASICSTRING_CLASS_H
#define DEF_BASICSTRING_CLASS_H

#include <common.h>

template <typename T> class Vector;

template <typename T>
class BasicString {
	protected:
	T *m_string;
	u32int m_length;

	public:
	BasicString();
	BasicString(const T* string, u32int length);
	BasicString(const BasicString<T> &other);
	BasicString(const T, u32int count = 1);
	virtual ~BasicString();

	void affect(const BasicString<T> &other);
	void affect(const T* string, u32int length);
	void affect(const T value, u32int count = 1);
	void operator= (const BasicString<T> &other) { affect(other); }

	bool compare(const BasicString<T> &other) const;
	bool compare(const T* string, u32int length) const;
	bool operator== (const BasicString<T> &other) const { return compare(other); }
	bool operator!= (const BasicString<T> &other) const { return !compare(other); }

	BasicString<T>& append(const BasicString<T> &other);
	BasicString<T>& append(const T* string, u32int length);
	BasicString<T>& append(const T other);
	BasicString<T>& operator+= (const BasicString<T> &other) { return append(other); }
	BasicString<T>& operator+= (const T other) { return append(other); }
	
	BasicString<T> concat(const BasicString<T> &other) const;
	BasicString<T> concat(const T* string, u32int length) const;
	BasicString<T> concat(const T other) const;

	T& operator[] (int index) const { return m_string[index]; }

	u32int size() const { return m_length; }
	void clear();
	bool empty() const { return m_length == 0; }
	bool contains(const T& chr) const;

	Vector< BasicString<T> > split(T sep) const;
	BasicString<T> substr(s32int start, s32int size = -1);
};

#include "BasicString.class.cpp"

#endif
