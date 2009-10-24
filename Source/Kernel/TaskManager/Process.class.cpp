#include "Process.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/File.class.h>
#include <Linker/Binary.proto.h>
#include <Process.iface.h>
#include <UserManager/Usr.ns.h>
#include <VirtualTerminal.iface.h>
#include <SyscallManager/Res.ns.h>

#define ISPARENT Task::currProcess()->getPid() == m_ppid

namespace Mem {
	extern Heap kheap;
}

call_t Process::m_callTable[] = {
	CALL0(PRIF_EXIT, &Process::exitSC),
	CALL1(PRIF_ALLOCPAGE, &Process::allocPageSC),
	CALL1(PRIF_FREEPAGE, &Process::freePageSC),
	CALL0(PRIF_GETPID, &Process::getPid),
	CALL0(PRIF_GETPPID, &Process::getPpid),
	CALL0(PRIF_ARGC, &Process::argcSC),
	CALL1(PRIF_ARGV, &Process::argvSC),
	CALL0(PRIF_START, &Process::startSC),
	CALL1(PRIF_AUTODELETE, &Process::autoDeleteSC),
	CALL1(PRIF_PUSHARG, &Process::pushArgSC),
	CALL1(PRIF_SETOUTVT, &Process::setOutVTSC),
	CALL1(PRIF_SETINVT, &Process::setInVTSC),
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

Process::Process() : Ressource(PRIF_OBJTYPE, m_callTable) {	//Private constructor, does nothing
}

Process* Process::createKernel(String cmdline, VirtualTerminal *vt) {
	Process* p = new Process();
	p->m_pid = 0;
	p->m_ppid = 0;
	p->m_arguments = cmdline.split(" ");
	p->m_retval = 0;
	p->m_state = P_RUNNING;
	p->m_pagedir = kernelPageDirectory;
	p->m_uid = 0;
	p->m_userHeap = &Mem::kheap;
	p->m_inVT = p->m_outVT = vt;
	
	Thread* t = new Thread();
	t->m_process = p;
	t->m_state = T_RUNNING;
	t->m_isKernel = true;
	t->m_kernelStack.addr = 0;
	t->m_kernelStack.size = 0;

	p->registerThread(t);
	Task::registerProcess(p);
	Task::registerThread(t);

	return p;
}

Process* Process::run(String filename, u32int uid) {
	File file(filename, FM_READ, (FSNode*)Task::currProcess()->getCwd());
	if (!file.valid()) return 0;
	Binary* b = Binary::load(file);
	if (b == 0) return 0;
	Process* p = new Process(filename, uid);
	thread_entry_t e = b->toProcess(p);
	delete b;
	if (e != 0) {
		new Thread(p, e, 0);
		return p;
	} else {
		delete p;
		return 0;
	}
}

Process::Process(String binfile, u32int uid) : Ressource(PRIF_OBJTYPE, m_callTable), m_arguments() {
	m_pid = Task::nextPid();
	m_ppid = Task::currProcess()->getPid();
	m_arguments.push(binfile);
	m_retval = 0;
	m_state = P_STARTING;
	m_uid = uid;
	m_autodelete = false;
	m_cwd = Task::currProcess()->getCwd();
	m_inVT = Task::currProcess()->getInVT();
	m_outVT = Task::currProcess()->getOutVT();
	m_fileDescriptors = 0;
	//Create page directory and user heap
	m_pagedir = new PageDirectory(kernelPageDirectory);
	m_pagedir->switchTo();
	m_userHeap = new Heap();
	u32int heapIdxSize = PhysMem::total() * 16 + 0x10000;
	m_userHeap->create(USERHEAPSTART, USERHEAPINITSIZE + heapIdxSize, heapIdxSize, m_pagedir, true, true);
}

Process::~Process() {
	exit();	//Kill all threads
	delete m_pagedir;
	delete m_userHeap;
}

void Process::start() {
	if (m_state == P_STARTING) m_state = P_RUNNING;
}

void Process::pushArg(const String& arg) {
	m_arguments.push(arg);
}

void Process::exit() {
	for (u32int i = 0; i < m_threads.size(); i++) {
		delete m_threads[i];
	}
	m_threads.clear();
	while (m_fileDescriptors != 0) {
		m_fileDescriptors->v()->close(false);
		delete m_fileDescriptors->v();
		m_fileDescriptors = m_fileDescriptors->delThis();
	}
	m_state = P_FINISHED;
}

void Process::registerThread(Thread* t) {
	if (m_state != P_FINISHED)
		m_threads.push(t);
}

void Process::threadFinishes(Thread* thread, u32int retval) {
	// If it is the main thread of the process, or if it pagefaulted
	if (thread == m_threads[0] or retval == E_PAGEFAULT or retval == E_EXIT) {
		m_retval = retval;
		if (m_autodelete) {
			delete this;
		} else {
			exit();
		}
	} else {
		//Simply unregister thread
		for (u32int i = 0; i < m_threads.size(); i++) {
			if (m_threads[i] == thread) {
				m_threads[i] = m_threads.back();
				m_threads.pop();
				break;
			}
		}
		delete thread;
	}
}

void Process::registerFileDescriptor(File* fd) {
	m_fileDescriptors = m_fileDescriptors->cons(fd);
}

void Process::unregisterFileDescriptor(File* fd) {
	m_fileDescriptors = m_fileDescriptors->removeOnce(fd);
}

PageDirectory* Process::getPagedir() {
	return m_pagedir;
}

VirtualTerminal* Process::getInVT() {
	return m_inVT;
}

VirtualTerminal* Process::getOutVT() {
	return m_outVT;
}

void Process::setInVT(VirtualTerminal* vt) {
	m_inVT = vt;
}

void Process::setOutVT(VirtualTerminal* vt) {
	m_outVT = vt;
}

u32int Process::exitSC() {
	if (Task::currProcess() != this) return 1;
	Task::currentThreadExits(E_EXIT);
	return 0;
}

u32int Process::allocPageSC(u32int pos) {
	if (Task::currProcess() != this) return 1;
	if ((pos & 0x00000FFF) != 0) pos = (pos & 0xFFFFF000) + 0x1000;
	if (pos >= 0xC0000000) return 1;
	m_pagedir->allocFrame(pos, true, true);
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

u32int Process::freePageSC(u32int pos) {
	if (Task::currProcess() != this) return 1;
	if ((pos & 0x00000FFF) != 0) pos = (pos & 0xFFFFF000) + 0x1000;
	if (pos >= 0xC0000000) return 1;
	m_pagedir->freeFrame(pos);
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


bool Process::accessible() {
	return (m_uid == Usr::uid());
}
