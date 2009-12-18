#ifndef DEF_VTUNIXFD_CLASS_H
#define DEF_VTUNIXFD_CLASS_H

#include "UnixFileDescriptor.proto.h"
#include <VTManager/VirtualTerminal.proto.h>

class VTUnixFD : public UnixFileDescriptor {
	private:
	VirtualTerminal *m_vt;

	public:
	VTUnixFD(String name, VirtualTerminal *vt);

	int read(char *ptr, int len);
	int write(char *ptr, int len);
};

#endif
