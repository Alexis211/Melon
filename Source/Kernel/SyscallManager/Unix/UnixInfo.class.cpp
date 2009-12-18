#include "UnixInfo.class.h"

#include "VTUnixFD.class.h"

UnixInfo::UnixInfo(int pid, VirtualTerminal *invt, VirtualTerminal *outvt) {
	m_pid = pid;
	//TODO : add stderr, stdin, stdout
	m_fds.push(new VTUnixFD("/dev/stdin", invt));
	m_fds.push(new VTUnixFD("/dev/stdout", outvt));
	m_fds.push(new VTUnixFD("/dev/stderr", outvt));
}

UnixInfo::~UnixInfo() {
	for (u32int i = 0; i < m_fds.size(); i++) {
		if (m_fds[i] != 0) delete m_fds[i];
	}
}

UnixFileDescriptor* UnixInfo::getFd(int file) {
	if (file >= (int)m_fds.size()) return 0;
	if (file < 0) return 0;
	return m_fds[file];
}

int UnixInfo::setFd(UnixFileDescriptor* file) {
	for (u32int i = 0; i < m_fds.size(); i++) {
		if (m_fds[i] == 0) {
			m_fds[i] = file;
			return i;
		}
	}
	m_fds.push(file);
	return m_fds.size() - 1;
}

void UnixInfo::closeFd(int file) {
	if (file >= (int)m_fds.size()) return;
	if (file < 0) return;
	if (m_fds[file] == 0) return;
	delete m_fds[file];
	m_fds[file] = 0;
}
