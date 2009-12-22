#ifndef DEF_TASK_NS_H
#define DEF_TASK_NS_H

#include <TaskManager/Thread.class.h>
#include <VTManager/VirtualTerminal.proto.h>
#include <SimpleList.class.h>

namespace Task {
	Thread* currThread();
	Process* currProcess();

	extern SimpleList <Process*> *processes;
	extern SimpleList <Thread*> *threads;

	void initialize(String cmdline, VirtualTerminal *vt);	//cmdline should be the bootloader kernel command line, if anybody needs it
	void doSwitch();
	void triggerSwitch();
	u32int nextPid();
	bool IRQwakeup(u8int irq);

	Process* getKernelProcess();	//Returns first registered process

	void currentThreadExits(u32int errcode); 	//Syscall called when a thread finishes

	//These are used by the constructors/destructors of Thread and Process
	void registerThread(Thread* t);
	void unregisterThread(Thread* t);
	void registerProcess(Process* p);
	void unregisterProcess(Process* p);
}

#endif
