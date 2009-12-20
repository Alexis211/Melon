#ifndef DEF_STRINGSTREAM_CLASS_H
#define DEF_STRINGSTREAM_CLASS_H

#include <IStream.proto.h>
#include <OStream.proto.h>

class StringIStream : public IStream {
	private:
	SimpleList<String> *m_elements;

	String read();

	public:
	StringIStream(const String &e);
	StringIStream();
	~StringIStream();

	void append(const String &e);
};

class StringOStream : public OStream {
	private:
	String m_str;

	void write(const String& s) { m_str += s; }

	public:
	
	const String &str() const { return m_str; }
	void clear() { m_str = ""; }
};


#endif

