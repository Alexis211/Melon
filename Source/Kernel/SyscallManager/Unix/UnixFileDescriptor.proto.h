#ifndef DEF_UNIXFILEDESCRIPTOR_CLASS_H
#define DEF_UNIXFILEDESCRIPTOR_CLASS_H

#include <String.class.h>

class UnixFileDescriptor {
	private:
	String m_path;

	public:
	UnixFileDescriptor(String path) : m_path(path) {}
	virtual ~UnixFileDescriptor() {}

	virtual int read(char *ptr, int len) = 0;
	virtual int write(char *ptr, int len) = 0;
	virtual int lseek(int ptr, int dir) { return 0; }

	String getPath() { return m_path; }
};

#endif
