#ifndef DEF_STRING_CLASS
#define DEF_STRING_CLASS

#include <Core/common.wtf.h>

template <typename T> class Vector;

class String {
	private:
	char *m_string;
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
	String &operator+= (char other);
	String &operator+ (String &other);
	String &operator+ (char* other);
	String &operator+ (char other);
	operator char* ();
	s32int toInt();
	u32int toInt16();	//From HEX
	char& operator[] (int index);

	u32int size();
	void clear();
	bool empty();

	Vector<String> split(char c);

	String substr(s32int start, s32int size);
};

#endif
