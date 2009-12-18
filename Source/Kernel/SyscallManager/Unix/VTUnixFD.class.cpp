#include "VTUnixFD.class.h"

VTUnixFD::VTUnixFD(String name, VirtualTerminal *vt) : UnixFileDescriptor(name) {
	m_vt = vt;
}

int VTUnixFD::read(char *ptr, int len) {
	int i;
	for (i = 0; i < len; i++) {
		keypress_t kp = m_vt->getKeypress();
		if (kp.haschar) {
			if (kp.hascmd) {
				ptr[i] = '^';
				i++;
			}
			uchar_repr_t t = kp.character.encode();
			for (int j = 0; j < CHARSZ(t); j++) {
				ptr[i] = t.c[j];
				i++;
			}
			i--;
		} else if (kp.hascmd) {
			if (kp.command == KBDC_ENTER) {
				ptr[i] = '\n';
				return i;
			} else if (kp.command == KBDC_TAB) {
				ptr[i] = '\t';
				i++;
			} else if (kp.command == KBDC_BACKSPACE) {
				i--;
			}
		}
	}
	return i;
}

int VTUnixFD::write(char *ptr, int len) {
	u8int b[4];
	for (int i = 0; i < 4; i++) {
		b[i] = ptr[len + i];
		ptr[len + i] = 0;
	}
	*m_vt << ptr;
	for (int i = 0; i < 4; i++) ptr[len + i] = b[i];
	return len;
}
