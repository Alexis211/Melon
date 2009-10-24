#ifndef DEF_FILEVT_CLASS_H
#define DEF_FILEVT_CLASS_H

#include <TextFile.class.h>
#include <VTManager/VirtualTerminal.proto.h>

class FileVT : public VirtualTerminal {
	protected:
	TextFile m_file;
	bool m_isWriting;	//True = write only, false = read only
	String m_buffer;
	u32int m_bufferPos;

	public:
	FileVT(String filename, u8int mode = FM_READ, FSNode* start = 0, u8int encoding = UE_UTF8);

	bool isBoxed() { return false; }
	void setCursorCol(u32int col);

	void put(WChar c, bool updatecsr = true);
	Kbd::keypress_t getKeypress(bool show = true, bool block = true);
};

#endif
