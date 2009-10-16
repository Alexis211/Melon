#include "Thread.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <DeviceManager/Time.ns.h>

void runThread(Thread* thread, void* data, thread_entry_t entry_point) {
	asm volatile("sti");
	u32int ret = entry_point(data);	//Run !
	asm volatile("mov %0, %%eax; int $66;" : : "r"(ret));	//Syscall for thread ending
}

Thread::Thread() {	//Private constructor, does nothing
}

Thread::Thread(thread_entry_t entry_point, void* data, bool iskernel) {
	if (iskernel) {
		m_isKernel = true;
		u32int tmp;
		m_kernelStackFrame = (u32int)PageAlloc::alloc(&tmp);
		m_process = Task::getKernelProcess();
		setup(entry_point, data, m_kernelStackFrame + 0x1000);	//A kernel stack always is 1 frame, meaning 0x1000 bytes
	} else {
		m_isKernel = false;
		m_process = Task::currProcess();
		setup(entry_point,  data, m_process->stackAlloc() + STACKSIZE);
	}
}

Thread::Thread(Process* process, thread_entry_t entry_point, void* data) {
	m_isKernel = false;
	m_process = process;
	setup(entry_point, data, m_process->stackAlloc() + STACKSIZE);
}

Thread::~Thread() {
	Task::unregisterThread(this);
	if (m_isKernel) {
		PageAlloc::free((void*)m_kernelStackFrame);
	}
	//Don't unregister thread in process, it has probably already been done
}

void Thread::setup(thread_entry_t entry_point, void* data, u32int esp) {
	//Pass function parameters for runThread()
	u32int *stack = (u32int*)esp;
	stack--;
	*stack = (u32int)entry_point;	//Push entry point (function parameter)
	stack--;
	*stack = (u32int)data;	//Push data
	stack--;
	*stack = (u32int)this;	//Push object pointer
	stack--;
	*stack = 0;	//Null return address
	m_esp = (u32int)stack;
	m_ebp = m_esp + 8;
	m_eip = (u32int)runThread;

	m_state = T_RUNNING;
	m_process->registerThread(this);
	Task::registerThread(this);
}

void Thread::finish(u32int errcode) {
	if (errcode == E_PAGEFAULT and m_isKernel) {
		PANIC("Page fault in kernel thread !");
	}
	if (errcode == E_UNHANDLED_EXCEPTION and m_isKernel) {
		PANIC("Unhandled exception in kernel thread !");
	}
	//Needs not set m_state to a finished state, either :
	// - thread is unregistered from process and everywhere
	// - errcode is an exception or this is main thread, process exits
	m_process->threadFinishes(this, errcode);
} 

void Thread::handleException(registers_t regs, int no) {
	char* exceptions[] = {
		"Division by zero", "Debug exception", "Non maskable interrupt", 
		"Breakpoint exception", "'Into detected overflow'", "Out of bounds exception",
		"Invalid opcode exception", "No coprocessor exception", "Double fault",
		"Coprocessor segment overrun", "Bad TSS", "Segment not present",
		"Stack fault", "General protection fault", "Page fault",
		"Unknown", "Coprocessor fault", "Alignement check exception",
		"Machine check exception", "", "",
		"", "", "",
		"", "", "",
		"", "", "",
		"", ""};

	*(m_process->m_vt) << "\nUnhandled exception " << (s32int)no << " at " << (u32int)regs.cs << ":" <<
		(u32int)regs.eip << "\n:: " << exceptions[no];
	if (m_isKernel) PANIC_DUMP("Exception in kernel thread", &regs);

	if (no == 14) {	//Page fault
		int present = !(regs.err_code & 0x1);
		int rw = regs.err_code & 0x2;
		int us = regs.err_code & 0x4;
		int rsvd = regs.err_code & 0x8;
		u32int faddr;
		asm volatile("mov %%cr2, %0" : "=r"(faddr));
		*(m_process->m_vt) << "\n   ";
		if (present) *(m_process->m_vt) << "Present ";
		if (rw) *(m_process->m_vt) << "R/W ";
		if (us) *(m_process->m_vt) << "User ";
		if (rsvd) *(m_process->m_vt) << "Rsvd ";
		*(m_process->m_vt) << "At:" << (u32int)faddr;

		*(m_process->m_vt) << "\nThread finishing.\n";
		finish(E_PAGEFAULT);
		return;
	}
	*(m_process->m_vt) << "\nThread finishing.\n";
	finish(E_UNHANDLED_EXCEPTION);
}

void Thread::setState(u32int esp, u32int ebp, u32int eip) {
	m_esp = esp;
	m_ebp = ebp;
	m_eip = eip;
}

u32int Thread::getEsp() { return m_esp; }
u32int Thread::getEbp() { return m_ebp; }
u32int Thread::getEip() { return m_eip; }

Process* Thread::getProcess() { return m_process; }

void Thread::sleep(u32int msecs) {
	m_state = T_SLEEPING;
	waitfor.m_time = Time::time() + msecs;

	Task::triggerSwitch();
}

void Thread::waitIRQ(u8int irq) {
	m_state = T_IRQWAIT;
	waitfor.m_irq = irq;

	Task::triggerSwitch();
}

bool Thread::runnable() {
	if (m_state == T_RUNNING) return true;
	if (m_state == T_SLEEPING and Time::time() >= waitfor.m_time) {
		m_state = T_RUNNING;
		return true;
	}
	return false;
}
