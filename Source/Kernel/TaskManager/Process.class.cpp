#include "Process.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/File.class.h>
#include <Linker/Binary.proto.h>

namespace Mem {
	extern Heap kheap;
}

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
	p->m_userHeap = &Mem::kheap;
	p->m_vt = vt;
	
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

Process* Process::run(String filename, FSNode* cwd, u32int uid) {
	File file(filename, FM_READ, cwd);
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

Process::Process(String cmdline, u32int uid) {
	m_pid = Task::nextPid();
	m_cmdline = cmdline;
	m_retval = 0;
	m_state = P_STARTING;
	m_uid = uid;
	m_vt = Task::currProcess()->getVirtualTerminal();
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

void Process::exit() {
	for (u32int i = 0; i < m_threads.size(); i++) {
		delete m_threads[i];
	}
	m_threads.clear();
	for (SimpleList<File*> *iter = m_fileDescriptors; iter != 0; iter = iter->next()) {
		iter->v()->close(false);
		delete iter->v();
	}
	if (m_fileDescriptors != 0) delete m_fileDescriptors; //Will recursively delete whole list
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

VirtualTerminal* Process::getVirtualTerminal() {
	return m_vt;
}

void Process::setVirtualTerminal(VirtualTerminal* vt) {
	m_vt = vt;
}
