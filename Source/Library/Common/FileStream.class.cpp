#include "FileStream.class.h"

//************************************
//										INPUT FILE STREAM
//																********************

FileIStream::FileIStream(const String &filename, u8int encoding, FSNode start) : m_start(start) {
	m_file = new File(filename, FM_READ, start);
	m_filenames = 0;
	m_encoding = encoding;
}

FileIStream::FileIStream(u8int encoding, FSNode start) : m_start(start) {
	m_file = 0;
	m_filenames = 0;
	m_encoding = encoding;
}

FileIStream::~FileIStream() {
	if (m_file != 0) {
		m_file->close();
		delete m_file;
	}
}

void FileIStream::appendFile(const String &filename) {
	if (m_filenames == 0) {
		m_filenames = new SimpleList<String>(filename);
	} else {
		m_filenames->addAtEnd(filename);
	}
}

String FileIStream::read() {
	while (m_file == 0 or m_file->eof() or !m_file->valid()) {
		if (m_file != 0) {
			m_file->close();
			delete m_file;
		}
		m_file = 0;
		if (m_filenames == 0) {
			return "";
		} else {
			m_file = new File(m_filenames->v(), FM_READ, m_start);
			m_filenames = m_filenames->delThis();
		}
	}
	ByteArray temp;
	temp.resize(512);
	u32int r = m_file->read(temp);
	temp.resize(r);
	return temp.toString(m_encoding);
}

//************************************
//										OUTPUT FILE STREAM
//																********************

FileOStream::FileOStream(const String& filename, u8int mode, u8int encoding, FSNode start) {
	m_file = new File(filename, mode, start);
	m_encoding = encoding;
}

FileOStream::~FileOStream() {
	delete m_file;
}

void FileOStream::write(const String &s) {
	ByteArray a(s, m_encoding);
	m_file->write(a);
}
