#include "Process.class.h"
#include <TaskManager/Task.ns.h>
#include <Process.iface.h>
#include <UserManager/Usr.ns.h>
#include <VirtualTerminal.iface.h>
#include <SyscallManager/Res.ns.h>

#define ISPARENT Task::currProcess()->getPid() == m_ppid

call_t Process::m_callTable[] = {
	CALL0(PRIF_EXIT, &Process::exitSC),
	CALL1(PRIF_ALLOCPAGES, &Process::allocPagesSC),
	CALL1(PRIF_FREEPAGES, &Process::freePagesSC),
	CALL0(PRIF_GETPID, &Process::getPid),
	CALL0(PRIF_GETPPID, &Process::getPpid),
	CALL0(PRIF_ARGC, &Process::argcSC),
	CALL1(PRIF_ARGV, &Process::argvSC),
	CALL0(PRIF_START, &Process::startSC),
	CALL1(PRIF_AUTODELETE, &Process::autoDeleteSC),
	CALL1(PRIF_PUSHARG, &Process::pushArgSC),
	CALL1(PRIF_SETOUTVT, &Process::setOutVTSC),
	CALL1(PRIF_SETINVT, &Process::setInVTSC),
	CALL2(PRIF_AUTHPW, &Process::authPWSC),
	CALL1(PRIF_AUTHNOPW, &Process::authNoPWSC),
	CALL0(0, 0)
};

u32int Process::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == PRIF_SGETCPR) return Task::currProcess()->resId();
	if (wat == PRIF_SRUN) {
		String* e = (String*)a;
		Process* p = Process::run(*e, Usr::uid());
		if (p != 0) return p->resId();
	}
	if (wat == PRIF_SWAIT) {
		Process* p = Res::get<Process>(a, PRIF_OBJTYPE);
		if (Task::currProcess()->getPid() != p->m_ppid) return 0;
		while (p->m_state != P_FINISHED and !p->m_autodelete) Task::currThread()->sleep(20);
		if (p->m_autodelete) return E_AUTODELETE;
		s32int ret = p->m_retval;
		delete p;
		return ret;
	}
	return (u32int) - 1;
}

u32int Process::exitSC() {
	if (Task::currProcess() != this) return 1;
	Task::currentThreadExits(E_EXIT);
	return 0;
}

u32int Process::allocPagesSC(u32int pos, u32int count) {
	if (Task::currProcess() != this) return 1;
	if ((pos & 0x00000FFF) != 0) pos = (pos & 0xFFFFF000) + 0x1000;
	if (pos + (count - 1) * 0x1000 >= 0xC0000000) return 1;
	for (u32int i = 0; i < count; i++)
		m_pagedir->allocFrame(pos + (i * 0x1000), true, true);
	return 0;
}

u32int Process::argcSC() {
	if (Usr::uid() == m_uid or ISROOT or ISPARENT) {
		return (m_arguments.size());
	}
	return (u32int) - 1;
}

u32int Process::argvSC(u32int idx) {
	if (idx >= m_arguments.size()) return (u32int) - 1;
	if (Usr::uid() == m_uid or ISROOT or ISPARENT) {
		return m_arguments[idx].serialize();
	}
	return (u32int) - 1;
}

u32int Process::freePagesSC(u32int pos, u32int count) {
	if (Task::currProcess() != this) return 1;
	if ((pos & 0x00000FFF) != 0) pos = (pos & 0xFFFFF000) + 0x1000;
	if (pos + (count - 1) * 0x1000 >= 0xC0000000) return 1;
	for (u32int i = 0; i < count; i++)
		m_pagedir->freeFrame(pos + (i * 0x1000));
	return 0;
}

u32int Process::startSC() {
	if (Task::currProcess()->getPid() == m_ppid) {
		start();
		return 1;
	} 
	return 0;
}

u32int Process::autoDeleteSC(u32int d) {
	if (Task::currProcess()->getPid() != m_ppid) return 2;
	m_autodelete = (d != 0);
	return (m_autodelete ? 1 : 0);
}

u32int Process::pushArgSC(u32int arg) {
	String* a = (String*)arg;
	m_arguments.push(*a);
	return 0;
}

u32int Process::setInVTSC(u32int vtid) {
	if (Task::currProcess()->getPid() != m_ppid) return 0;
	VirtualTerminal* vt = Res::get<VirtualTerminal>(vtid, VTIF_OBJTYPE);
	if (vt != 0) setInVT(vt);
	return 1;
}

u32int Process::setOutVTSC(u32int vtid) {
	if (Task::currProcess()->getPid() != m_ppid) return 0;
	VirtualTerminal* vt = Res::get<VirtualTerminal>(vtid, VTIF_OBJTYPE);
	if (vt != 0) setOutVT(vt);
	return 1;
}

u32int Process::authPWSC(u32int user, u32int pw) {
	if (Task::currProcess() != this) return 0;
	String* u = (String*)user;
	String* p = (String*)pw;
	User* usr = Usr::user(*u);
	if (usr == 0) return 0;
	if (usr->vrfyPassword(*p)) {
		m_uid = usr->getUid();
		return 1;
	}
	return 0;
}

u32int Process::authNoPWSC(u32int user) {
	if (Task::currProcess() != this) return 0;
	if (!ISROOT) return 0;
	String* u = (String*)user;
	User* usr = Usr::user(*u);
	if (usr == 0) return 0;
	m_uid = usr->getUid();
	return 1;
}

bool Process::accessible() {
	return (m_uid == Usr::uid());
}
