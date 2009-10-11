#include "Task.ns.h"
#include <Library/Vector.class.h>

//From Task.wtf.asm
extern "C" u32int read_eip();
extern "C" u32int idle_task(void*);

namespace Task {

SimpleList <Process*> *processes = 0;	//TODO : use a linked list instead
SimpleList <Thread*> *threads = 0;

SimpleList <Thread*> *currentThread = 0;
Process* currentProcess = 0;
SimpleList<Thread*> *idleThread = 0;

u32int nextpid = 1;

struct finished_thread_t {	//Forms a linked list
	Thread* thread;
	u32int errcode;
};
SimpleList<finished_thread_t> *firstFinishedThread = 0;


Thread* currThread() {
	return currentThread->v();
}

Process* currProcess() {
	return currentProcess;
}

Process* getKernelProcess() {
	if (processes == 0) PANIC("kko");
	return processes->last()->v();
}

void initialize(String cmdline, VirtualTerminal *vt) {
	asm volatile ("cli");
	threads = 0;
	processes = 0;
	currentProcess = Process::createKernel(cmdline, vt);
	currentThread = threads;
	Thread* idle = new Thread(idle_task, 0, true);
	for (SimpleList<Thread*> *iter = threads; iter != 0; iter = iter->next()) {
		if (iter->v() == idle) {
			idleThread = iter;
			break;
		}
	}
	asm volatile ("sti");
}

SimpleList<Thread*> *nextThread() {
	//Clean up finished threads
	while (firstFinishedThread != 0) {
		if (firstFinishedThread->v().thread == currentThread->v()) break;
		firstFinishedThread->v().thread->finish(firstFinishedThread->v().errcode);
		firstFinishedThread = firstFinishedThread->delThis();
	}

	//Find next thread
	SimpleList<Thread*> *iter = currentThread;
	while (1) {
		iter = iter->next();
		if (iter == 0) iter = threads;
		if (iter->v()->runnable() and iter->v() != idleThread->v()) {
			if (firstFinishedThread != 0 and firstFinishedThread->v().thread == iter->v()) return idleThread;
			return iter;
		}
		if (iter == currentThread) break;
	}
	return idleThread;
}

void doSwitch() {
	if (currentThread == NULL or currentProcess == NULL) return;

	u32int esp, ebp, eip, cr3;

	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	eip = read_eip();

	if (eip == 0x12345) {
		return;
	}

	if ((u32int)currentThread->v() != 0xFFFFFFFF) currentThread->v()->setState(esp, ebp, eip);

	currentThread = nextThread();
	Thread* t = currentThread->v();
	currentProcess = t->getProcess();

	esp = t->getEsp();
	ebp = t->getEbp();
	eip = t->getEip();
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
	for (SimpleList<Thread*> *iter = threads; iter != 0; iter = iter->next()) {
		r = r or iter->v()->irqHappens(irq);
	}
	return r;
}

void allocKernelPageTable(u32int id, page_table_t *table, u32int tablePhys) {
	if (id < 768) return;	//this would be a BUG
	for (SimpleList<Process*> *iter = processes; iter != 0; iter = iter->next()) {
		iter->v()->getPagedir()->tables[id] = table;
		iter->v()->getPagedir()->tablesPhysical[id] = tablePhys;
	}
}

void currentThreadExits(u32int errcode) {
	finished_thread_t tmp = {currentThread->v(), errcode};
	firstFinishedThread = firstFinishedThread->cons(tmp);
}

void registerThread(Thread* t) {
	unregisterThread(t);	//...//
	threads = threads->cons(t);
}

void unregisterThread(Thread* t) {
	if (threads == 0) return;	//Tasking not yet initialized
	if (threads->v() == t) {
		threads = threads->delThis();
		return;
	}
	for (SimpleList<Thread*> *iter = threads; iter->next() != 0; iter = iter->next()) {
		if (iter->next()->v() == t) {
			iter->delNext();
			return;
		}
	}
}

void registerProcess(Process* p) {
	unregisterProcess(p);	//...//
	processes = processes->cons(p);
}

void unregisterProcess(Process* p) {
	if (processes == 0) return; //Tasking not yet initialized
	if (processes->v() == p) {
		processes = processes->delThis();
		return;
	}
	for (SimpleList<Process*> *iter = processes; iter->next() != 0; iter = iter->next()) {
		if (iter->next()->v() == p) {
			iter->delNext();
			return;
		}
	}
}

}

