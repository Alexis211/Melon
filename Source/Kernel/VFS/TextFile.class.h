#ifndef DEF_TEXTFILE_CLASS_H
#define DEF_TEXTFILE_CLASS_H

#include <VFS/File.class.h>

class TextFile : public File {
	private:
	u8int m_encoding;

	public:
	TextFile(u8int encoding = UE_UTF8) : File() { m_encoding = encoding; }
	TextFile(String filename, u8int mode = FM_READ, FSNode* start = 0, u8int encoding = UE_UTF8)
	   	: File(filename, mode, start) { m_encoding = encoding; }
	~TextFile() {}

	void setEncoding(u8int encoding = UE_UTF8) { m_encoding = encoding; }
	bool write(String str, bool addnl = false);	// Addnl = wether or not to add \n at end
	String readLine(char separator = '\n');
};

#endif
