#include "TextFile.class.h"

bool TextFile::write(String str, bool addnl) {
	ByteArray a(str, m_encoding);
	if (addnl) a += (u8int)'\n';
	return File::write(a);
}

String TextFile::readLine(char separator) {
	ByteArray buffer;
	while (1) {
		char c;
		if (read(1, (u8int*)&c) == 0) {
			return buffer.toString(m_encoding);
		}
		if (c == separator) {
			return buffer.toString(m_encoding);
		}
		buffer += (u8int)c;
	}
}
