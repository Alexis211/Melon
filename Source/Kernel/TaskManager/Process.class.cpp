#include "Process.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/File.class.h>

Process::Process() {	//Private constructor, does nothing
}

Process* Process::createKernel(String cmdline, VirtualTerminal *vt) {
	Process* p = new Process();
	p->m_pid = 0;
	p->m_cmdline = cmdline;
	p->m_retval = 0;
	p->m_state = P_RUNNING;
	p->m_pagedir = kernelPageDirectory;
	p->m_uid = 0;
	p->m_stacksstart = 0;
	p->m_vt = vt;
	
	Thread* t = new Thread();
	t->m_process = p;
	t->m_state = T_RUNNING;
	t->m_isKernel = true;
	t->m_kernelStackFrame = 0;

	p->registerThread(t);
	Task::registerProcess(p);
	Task::registerThread(t);

	return p;
}

Process::Process(String cmdline, u32int uid) {
	m_pid = Task::nextPid();
	m_cmdline = cmdline;
	m_retval = 0;
	m_state = P_RUNNING;
	m_pagedir = new PageDirectory(kernelPageDirectory);
	m_uid = uid;
	m_vt = Task::currentProcess->getVirtualTerminal();
	m_stacksstart = 0xC0000000;
}

Process::~Process() {
	exit();	//Kill all threads
	delete m_pagedir;
}

u32int Process::stackAlloc() {
	if (m_stacksstart < STACKSIZE) return 0;
	for (u32int i = m_stacksstart - STACKSIZE; i < m_stacksstart; i += 0x1000) {
		m_pagedir->allocFrame(i & 0xFFFFF000, true, true);
	}
	m_stacksstart -= STACKSIZE;
	return m_stacksstart;
}

void Process::exit() {
	while (!m_threads.empty()) {
		delete m_threads.back();
		m_threads.pop();
	}
	for (u32int i = 0; i < m_fileDescriptors.size(); i++) {
		m_fileDescriptors[i]->close(false);
		delete m_fileDescriptors[i];
	}
	m_fileDescriptors.clear();
	m_state = P_FINISHED;
}

void Process::registerThread(Thread* t) {
	if (m_state != P_FINISHED)
		m_threads.push(t);
}

void Process::threadFinishes(Thread* thread, u32int retval) {
	// If it is the main thread of the process, or if it pagefaulted
	if (thread == m_threads[0] or retval == E_PAGEFAULT) {
		exit();
	} else {
		//Simply unregister thread
		for (u32int i = 0; i < m_threads.size(); i++) {
			if (m_threads[i] == thread) {
				m_threads[i] = m_threads.back();
				m_threads.pop();
			}
		}
	}
}

void Process::registerFileDescriptor(File* fd) {
	m_fileDescriptors.push(fd);
}

void Process::unregisterFileDescriptor(File* fd) {
	for (u32int i = 0; i < m_fileDescriptors.size(); i++) {
		if (m_fileDescriptors[i] == fd) {
			m_fileDescriptors[i] = m_fileDescriptors.back();
			m_fileDescriptors.pop();
			break;
		}
	}
}

PageDirectory* Process::getPagedir() {
	return m_pagedir;
}

VirtualTerminal* Process::getVirtualTerminal() {
	return m_vt;
}

void Process::setVirtualTerminal(VirtualTerminal* vt) {
	m_vt = vt;
}
