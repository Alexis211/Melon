#include "VTUnixFD.class.h"

VTUnixFD::VTUnixFD(String name, VirtualTerminal *vt) : UnixFileDescriptor(name) {
	m_vt = vt;
}

int VTUnixFD::read(char *ptr, int len) {
	return 0;
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
