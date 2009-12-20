#ifndef DEF_ISTREAM_PROTO_h
#define DEF_ISTREAM_PROTO_h

#include <String.class.h>
#include <SimpleList.class.h>
#include <Mutex.class.h>

class IStream : private Mutex {
	private:
	SimpleList<String> *m_buffer;
	int m_ptr;
	void operator =(IStream& other);

	bool populate();

	protected:
	virtual String read() = 0;

	public:
	IStream();
	IStream(const IStream& other);
	virtual ~IStream();

	WChar getChar();
	String get(WChar delimiter = "\n");
	String getWord() { return get(" "); }
};

#endif
