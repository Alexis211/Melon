#include "TextFile.class.h"

bool TextFile::write(String str, bool addnl) {
	ByteArray a(str, m_encoding);
	if (addnl) a += (u8int)'\n';
	return File::write(a);
}

String TextFile::readLine(char separator) {
	const u32int bufflen = 512;
	String ret;
	ByteArray temp;
	while (1) {
		temp.resize(bufflen);
		u32int r = read(temp);
		u32int l = r;
		for (u32int i = 0; i < r; i++) {
			if (temp[i] == separator) {
				l = i;
				temp.resize(i);
				break;
			}
		}
		ret += temp.toString(m_encoding);
		if (l != r or r != bufflen) {
			if (l != r) seek((r - l) - 1, SM_BACKWARD);
			return ret;
		}
	}
}
