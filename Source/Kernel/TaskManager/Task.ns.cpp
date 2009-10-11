#include "Task.ns.h"
#include <Library/Vector.class.h>
#include <Library/SimpleList.class.h>

//From Task.wtf.asm
extern "C" u32int read_eip();
extern "C" u32int idle_task(void*);

namespace Task {

Vector <Process*> processes;	//TODO : use a linked list instead
Vector <Thread*> threads;

struct finished_thread_t {	//Forms a linked list
	Thread* thread;
	u32int errcode;
};

SimpleList<finished_thread_t> *firstFinishedThread = 0;

Thread* currentThread = NULL;
Process* currentProcess = NULL;
Thread* idleThread = NULL;
u32int currentThreadId = 0;

u32int nextpid = 1;

void initialize(String cmdline, VirtualTerminal *vt) {
	asm volatile ("cli");
	threads.clear();
	processes.clear();
	currentProcess = Process::createKernel(cmdline, vt);
	idleThread = new Thread(idle_task, 0, true);
	currentThread = threads[0];
	currentThreadId = 0;
	asm volatile ("sti");
}

Thread* nextThread() {
	//Clean up finished threads
	while (firstFinishedThread != 0) {
		DEBUG_HEX((u32int)firstFinishedThread);
		if (firstFinishedThread->v().thread == currentThread) break;
		firstFinishedThread->v().thread->finish(firstFinishedThread->v().errcode);
		firstFinishedThread = firstFinishedThread->delThis();
	}
	for (u32int i = 0; i < threads.size(); i++) {
		if (threads[i] == currentThread) {
			currentThreadId = i;
		}
	}
	
	//Find next thread
	u32int nid = currentThreadId;
	while (1) {
		nid++;
		if (nid >= threads.size()) nid = 0;
		if (threads[nid]->runnable() and threads[nid] != idleThread) {
			if (firstFinishedThread != 0 and firstFinishedThread->v().thread == threads[nid]) return idleThread;
			currentThreadId = nid;
			return threads[nid];
		}
		if (nid == currentThreadId) break;
	}
	return idleThread;
}

void doSwitch() {
	if (currentThread == NULL or currentProcess == NULL) return;

	u32int esp, ebp, eip, cr3;

	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	eip = read_eip();

	if (eip == 0x12345)
		return;

	if ((u32int)currentThread != 0xFFFFFFFF) currentThread->setState(esp, ebp, eip);

	currentThread = nextThread();
	currentProcess = currentThread->getProcess();

	esp = currentThread->getEsp();
	ebp = currentThread->getEbp();
	eip = currentThread->getEip();
	cr3 = currentProcess->getPagedir()->physicalAddr;

	asm volatile("			\
			cli;			\
			mov %0, %%ebp;	\
			mov %1, %%esp;	\
			mov %2, %%ecx;	\
			mov %3, %%cr3;	\
			mov $0x12345, %%eax;	\
			jmp *%%ecx;"
		: : "r"(ebp), "r"(esp), "r"(eip), "r"(cr3));
}

void triggerSwitch() {
	asm volatile("int $65");
}

u32int nextPid() {
	return nextpid++;
}

bool IRQwakeup(u8int irq) {
	bool r = false;
	for (u32int i = 0; i < threads.size(); i++) {
		r = r or threads[i]->irqHappens(irq);
	}
	return r;
}

void allocKernelPageTable(u32int id, page_table_t *table, u32int tablePhys) {
	if (id < 768) return;	//this would be a BUG
	for (u32int i = 1; i < processes.size(); i++) {
		processes[i]->getPagedir()->tables[id] = table;
		processes[i]->getPagedir()->tablesPhysical[id] = tablePhys;
	}
}

Process* getKernelProcess() {
	return processes[0];
}

void currentThreadExits(u32int errcode) {
	finished_thread_t tmp = {currentThread, errcode};
	firstFinishedThread = firstFinishedThread->cons(tmp);
}

void registerThread(Thread* t) {
	unregisterThread(t);	//...//
	threads.push(t);
}

void unregisterThread(Thread* t) {
	for (u32int i = 0; i < threads.size(); i++) {
		if (threads[i] == t) {
			threads[i] = threads.back();
			threads.pop();
			return;
		}
	}
}

void registerProcess(Process* p) {
	unregisterProcess(p);	//...//
	processes.push(p);
}

void unregisterProcess(Process* p) {
	for (u32int i = 0; i < processes.size(); i++) {
		if (processes[i] == p) {
			processes[i] = processes.back();
			processes.pop();
			return;
		}
	}
}

}

