#ifndef DEF_ISTREAM_PROTO_H
#define DEF_ISTREAM_PROTO_H

#include <String.class.h>
#include <SimpleList.class.h>
#include <Mutex.class.h>

enum ostream_modifiers_e {
	FLUSH,
	ENDL
};

class OStream : private Mutex {
	private:
	SimpleList<String> *m_buffer;
	SimpleList<String> *m_last;
	void operator =(OStream& other);

	protected:
	virtual void write(const String &s) = 0;
	
	public:
	OStream();
	OStream(const OStream& other);
	virtual ~OStream() { flush(); }

	void put(const String& s);
	void flush();
	
	//Formatting functions
	OStream& operator << (const String& s) { put(s); if (s.contains("\n")) flush(); return *this; }
	OStream& operator << (s64int i) { put(String::number(i)); return *this; }
	OStream& operator << (s32int i) { put(String::number(i)); return *this; }
	OStream& operator << (u32int i) { put(String::hex(i)); return *this; }
	OStream& operator << (ostream_modifiers_e m);
};

#endif
