#ifndef DEF_STRING_CLASS
#define DEF_STRING_CLASS

#include <BasicString.class.h>
#include <WChar.class.h>

class String : public BasicString<WChar> {
	public:
	static String hex(u32int number);
	static String number(s32int number);

	static String unserialize(u32int w);
	u32int serialize() const;

	String(const char* string, u8int encoding = UE_UTF8);
	String() : BasicString<WChar>() {}
	String(const String &other) : BasicString<WChar> (other) {}

	void affect(const char* string, u8int encoding = UE_UTF8);
	void operator= (const char* other) { affect(other); }
	void operator= (const String& other) { BasicString<WChar>::affect(other); }

	bool compare(const char* string, u8int encoding = UE_UTF8) const;
	bool operator== (const char* other) const { return compare(other); }
	bool operator!= (const char* other) { return !compare(other); }
	bool operator== (const String& other) const { return BasicString<WChar>::compare(other); }
	bool operator!= (const String& other) const { return !BasicString<WChar>::compare(other); }

	String& append(const char* other, u8int encoding = UE_UTF8);
	String &operator+= (const String &other) { BasicString<WChar>::append(other); return *this; }
	String &operator+= (const char* other) { return append(other); }
	String &operator+= (WChar other) { BasicString<WChar>::append(other); return *this; }

	String concat(const String &other) const;
	String concat(const char* other, u8int encoding = UE_UTF8) const;
	String concat(WChar other) const;
	String operator+ (const String &other) const { return concat(other); }
	String operator+ (const char* other) const { return concat(other); }
	String operator+ (WChar other) const { return concat(other); }
	
	s64int toInt() const; 	//Convert from DEC
	u64int toInt16() const;	//Convert from HEX

	Vector<String> split(WChar c) const;

	String substr(s32int start, s32int size = -1);
};

#endif
