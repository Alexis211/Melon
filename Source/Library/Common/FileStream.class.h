#ifndef DEF_FILESTREAM_CLASS_H
#define DEF_FILESTREAM_CLASS_H

#include <IStream.proto.h>
#include <OStream.proto.h>

#ifdef THIS_IS_MELON_KERNEL
#include <VFS/File.class.h>
#else
#include <Binding/File.class.h>
#endif

class FileIStream : public IStream {
	private:
	String read();

	SimpleList<String> *m_filenames;

	File *m_file;
	u8int m_encoding;
	
	public:
	FileIStream(const String &filename, u8int encoding = UE_UTF8, FSNode start = FSNode(0));
	~FileIStream();

	void appendFile(const String &filename);
};

class FileOStream : public OStream {
	private:
	File *m_file;
	u8int m_encoding;

	public:
	FileOStream(const String &filename, u8int mode, u8int encoding = UE_UTF8, FSNode start = FSNode(0));
	~FileOStream();

	void write(const String &s);
};

#endif
