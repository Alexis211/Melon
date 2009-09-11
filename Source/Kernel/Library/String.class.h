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

	String(char* string);
	String();
	String(const String &other);
	~String();

	void operator= (const String &other);
	void operator= (char* string);

	bool operator== (String &other);
	bool operator== (char* string);
	String &operator+= (String &other);
	String &operator+= (char* other);
	String &operator+= (WChar other);
	String &operator+ (String &other);
	String &operator+ (char* other);
	String &operator+ (WChar other);
	s32int toInt();
	u32int toInt16();	//From HEX
	WChar& operator[] (int index);

	u32int size();
	void clear();
	bool empty();

	Vector<String> split(WChar c);

	String substr(s32int start, s32int size);
};

#endif
