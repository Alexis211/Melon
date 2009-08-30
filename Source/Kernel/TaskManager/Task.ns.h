#ifndef DEF_TASK_NS_H
#define DEF_TASK_NS_H

#include <TaskManager/Thread.class.h>
#include <VTManager/VirtualTerminal.class.h>

namespace Task {
	extern Thread* currentThread;
	extern Process* currentProcess;

	void initialize(String cmdline, VirtualTerminal *vt);	//cmdline should be the bootloader kernel command line, if anybody needs it
	void doSwitch();
	void triggerSwitch();
	u32int nextPid();
	bool IRQwakeup(u8int irq);

	void allocKernelPageTable(u32int id, page_table_t *table, u32int tablePhys);
	Process* getKernelProcess();	//Returns first registered process

	//These are used by the constructors/destructors of Thread and Process
	void registerThread(Thread* t);
	void unregisterThread(Thread* t);
	void registerProcess(Process* p);
	void unregisterProcess(Process* p);
}

#endif
