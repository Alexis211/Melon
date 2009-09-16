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

	String(const char* string, u8int encoding = UE_UTF8);
	String();
	String(const String &other);
	~String();

	void affect(const String &other);
	void affect(const char* string, u8int encoding = UE_UTF8);
	void operator= (const String &other) { affect(other); }
	void operator= (const char* other) { affect(other); }

	bool compare(const String &other) const;
	bool compare(const char* string, u8int encoding = UE_UTF8) const;
	bool operator== (const String &other) const { return compare(other); }
	bool operator== (const char* other) const { return compare(other); }
	bool operator!= (const String &other) { return !compare(other); }
	bool operator!= (const char* other) { return !compare(other); }

	String& append(const String &other);
	String& append(const char* other, u8int encoding = UE_UTF8);
	String& append(WChar other);
	String &operator+= (const String &other) { return append(other); }
	String &operator+= (const char* other) { return append(other); }
	String &operator+= (WChar other) { return append(other); }

	String concat(const String &other) const;
	String concat(const char* other, u8int encoding = UE_UTF8) const;
	String concat(WChar other) const;
	String operator+ (const String &other) const { return concat(other); }
	String operator+ (const char* other) const { return concat(other); }
	String operator+ (WChar other) const { return concat(other); }

	s32int toInt() const; 	//Convert from DEC
	u32int toInt16() const;	//Convert from HEX

	WChar& operator[] (int index) const;

	u32int size() const;
	void clear();
	bool empty() const;

	Vector<String> split(WChar c) const;

	String substr(s32int start, s32int size);
};

#endif
