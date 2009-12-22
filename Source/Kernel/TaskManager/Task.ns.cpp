#include "Task.ns.h"
#include <MemoryManager/PhysMem.ns.h>

#define INVALID_TASK_MAGIC 0xBEEFFEED
#define TEMP_STACK_SIZE 256 //This must be big enough so that we can call all we need to call when a task finishes

//From Task.wtf.asm
extern "C" u32int read_eip();
extern "C" u32int idle_task(void*);

namespace Task {

SimpleList <Process*> *processes = 0;
SimpleList <Thread*> *threads = 0;

SimpleList <Thread*> *currentThread = 0;
Process* currentProcess = 0;
SimpleList<Thread*> *idleThread = 0;

u32int nextpid = 1;

u32int temp_stack[TEMP_STACK_SIZE];	//Temporary stack used for finishing current thread

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
	//Find next thread
	if ((u32int)currentThread == INVALID_TASK_MAGIC) {
		currentThread = threads;	//This will happen when we come here just after current thread has finished
	}
	SimpleList<Thread*> *iter = currentThread;
	while (1) {
		iter = iter->next();
		if (iter == 0) iter = threads;
		if (iter->v()->runnable() and iter->v() != idleThread->v()) {
			return iter;
		}
		if (iter == currentThread) break;
	}
	return idleThread;
}

void doSwitch() {
	if (currentThread == NULL or currentProcess == NULL) return;

	u32int esp, ebp, eip;

	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	eip = read_eip();

	if (eip == 0x12345) {
		return;
	}

	//This will happen when we come here just after current thread has finished
	if ((u32int)currentThread != INVALID_TASK_MAGIC) currentThread->v()->setState(esp, ebp, eip);

	currentThread = nextThread();
	Thread* t = currentThread->v();
	currentProcess = t->getProcess();

	esp = t->getEsp();
	ebp = t->getEbp();
	eip = t->getEip();

	asm volatile("cli");

	t->setKernelStack();

	asm volatile("			\
			mov %0, %%ebp;	\
			mov %1, %%esp;	\
			mov %2, %%ecx;	\
			mov $0x12345, %%eax;	\
			jmp *%%ecx;"
		: : "r"(ebp), "r"(esp), "r"(eip));
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

void currThreadExitProceed(u32int errcode) {
	currentThread->v()->finish(errcode);
	currentThread = (SimpleList<Thread*>*)INVALID_TASK_MAGIC;
	doSwitch();  //Normally never come back from here
}

void currentThreadExits(u32int errcode) {	//Call currThreadExitProceed with a working stack (we use temp_stack)
	asm volatile("cli");
	u32int* stack = &temp_stack[TEMP_STACK_SIZE];
	stack--;
	*stack = errcode;
	stack--;
	*stack = 0;
	u32int esp = (u32int)(stack), ebp = (u32int)(stack + 1), eip = (u32int)currThreadExitProceed;

	asm volatile("			\
			mov %0, %%ebp;	\
			mov %1, %%esp;	\
			mov %2, %%ecx;	\
			mov %3, %%cr3;	\
			jmp *%%ecx;"
		: : "r"(ebp), "r"(esp), "r"(eip), "r"(kernelPageDirectory->physicalAddr));
}

void registerThread(Thread* t) {
	unregisterThread(t);	//...//
	threads = threads->cons(t);
}

void unregisterThread(Thread* t) {
	if (threads == 0) return;	//Tasking not yet initialized
	threads = threads->removeOnce(t);
}

void registerProcess(Process* p) {
	unregisterProcess(p);	//...//
	processes = processes->cons(p);
}

void unregisterProcess(Process* p) {
	if (processes == 0) return; //Tasking not yet initialized
	processes = processes->removeOnce(p);
}

}

