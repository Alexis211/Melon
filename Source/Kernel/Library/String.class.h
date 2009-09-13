#ifndef DEF_STRING_CLASS
#define DEF_STRING_CLASS

#include <Core/common.wtf.h>
#include <Library/WChar.class.h>

template <typename T> class Vector;

class String {
	private:
	WChar *m_string;
	u32int m_length;

	public:
	static String hex(u32int number);
	static String number(s32int number);

	String(const char* string);
	String();
	String(const String &other);
	~String();

	void operator= (const String &other);
	void operator= (const char* string);

	bool operator== (const String &other) const;
	bool operator== (const char* string) const;
	bool operator!= (const String &other) { return !(operator== (other)); }
	bool operator!= (const char* other) { return !(operator== (other)); }
	String &operator+= (const String &other);
	String &operator+= (const char* other);
	String &operator+= (WChar other);
	String &operator+ (const String &other) const;
	String &operator+ (const char* other) const;
	String &operator+ (WChar other) const;
	s32int toInt() const;
	u32int toInt16() const;	//From HEX
	WChar& operator[] (int index) const;

	u32int size() const;
	void clear();
	bool empty() const;

	Vector<String> split(WChar c) const;

	String substr(s32int start, s32int size);
};

#endif
