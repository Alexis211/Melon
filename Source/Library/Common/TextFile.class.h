#ifndef DEF_TEXTFILE_CLASS_H
#define DEF_TEXTFILE_CLASS_H

#ifdef THIS_IS_MELON_KERNEL
#include <VFS/File.class.h>
#else
#include <Binding/File.class.h>
#endif

class TextFile : public File {
	private:
	u8int m_encoding;

	public:
#ifdef THIS_IS_MELON_KERNEL
	TextFile(u8int encoding = UE_UTF8) : File() { m_encoding = encoding; }
	TextFile(String filename, u8int mode = FM_READ, FSNode* start = 0, u8int encoding = UE_UTF8)
	   	: File(filename, mode, start) { m_encoding = encoding; }
#else
	TextFile(u32int id, u8int encoding = UE_UTF8) : File(id) { m_encoding = encoding; }
	TextFile(String filename, u8int mode = FM_READ, FSNode start = FSNode(0), u8int encoding = UE_UTF8)
	   	: File(filename, mode, start) { m_encoding = encoding; }
#endif
	~TextFile() {}

	void setEncoding(u8int encoding = UE_UTF8) { m_encoding = encoding; }
	bool write(String str, bool addnl = false);	// Addnl = wether or not to add \n at end
	String readLine(char separator = '\n');
};

#endif
