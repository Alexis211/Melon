#ifndef DEF_UNIXINFO_CLASS_H
#define DEF_UNIXINFO_CLASS_H

#include <Vector.class.h>
#include <VTManager/VirtualTerminal.proto.h>

#include "UnixFileDescriptor.proto.h"

class UnixInfo {
	private:
	int m_pid;

	Vector<UnixFileDescriptor*> m_fds;

	public:
	UnixInfo(int pid, VirtualTerminal *invt, VirtualTerminal *outvt);
	~UnixInfo();

	UnixFileDescriptor* getFd(int file);
	int setFd(UnixFileDescriptor* file);
	void closeFd(int file);
};

#endif
