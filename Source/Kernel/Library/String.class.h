#ifndef DEF_STRING_CLASS
#define DEF_STRING_CLASS

#include <Core/common.wtf.h>
#include <Library/WChar.class.h>
#include <Library/BasicString.class.h>

class String : public BasicString<WChar> {
	public:
	static String hex(u32int number);
	static String number(s32int number);

	String(const char* string, u8int encoding = UE_UTF8);
	String();
	String(const String &other);
	virtual ~String();

	void affect(const String &other) { BasicString<WChar>::affect(other); }
	void affect(const char* string, u8int encoding = UE_UTF8);
	void operator= (const String &other) { affect(other); }
	void operator= (const char* other) { affect(other); }

	bool compare(const String &other) const { return BasicString<WChar>::compare(other); }
	bool compare(const char* string, u8int encoding = UE_UTF8) const;
	bool operator== (const String &other) const { return compare(other); }
	bool operator== (const char* other) const { return compare(other); }
	bool operator!= (const String &other) { return !compare(other); }
	bool operator!= (const char* other) { return !compare(other); }

	String& append(const String &other) { BasicString<WChar>::append(other); return *this; }
	String& append(const char* other, u8int encoding = UE_UTF8);
	String& append(WChar other) { BasicString<WChar>::append(other); return *this; }
	String &operator+= (const String &other) { return append(other); }
	String &operator+= (const char* other) { return append(other); }
	String &operator+= (WChar other) { return append(other); }

	String concat(const String &other) const;
	String concat(const char* other, u8int encoding = UE_UTF8) const;
	String concat(WChar other) const;
	String operator+ (const String &other) const { return concat(other); }
	String operator+ (const char* other) const { return concat(other); }
	String operator+ (WChar other) const { return concat(other); }

	s64int toInt() const; 	//Convert from DEC
	u64int toInt16() const;	//Convert from HEX

	WChar& operator[] (int index) const;

	u32int size() const;
	void clear();
	bool empty() const;

	Vector<String> split(WChar c) const;

	String substr(s32int start, u32int size);
};

#endif
