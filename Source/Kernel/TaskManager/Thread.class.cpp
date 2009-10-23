#include "Thread.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <DeviceManager/Time.ns.h>
#include <MemoryManager/GDT.ns.h>

#include <Thread.iface.h>

call_t Thread::m_callTable[] = {
	CALL1(THIF_SLEEP, &Thread::sleepSC),
	CALL1(THIF_FINISH, &Thread::finishSC),
	CALL0(0, 0)
};

u32int Thread::scall(u8int wat, u32int a, u32int b, u32int c, u32int d) {
	if (wat == THIF_SGETCTH) return Task::currThread()->resId();
	return (u32int) - 1;
}

void runThread(Thread* thread, void* data, thread_entry_t entry_point) {
	thread->m_process->getPagedir()->switchTo();
	if (thread->m_isKernel) {
		asm volatile("sti");
		u32int ret = entry_point(data);	//Run !
		asm volatile("mov %0, %%eax; int $66;" : : "r"(ret));	//Syscall for thread ending
	} else {
		//Setup values on user stack
		u32int *stack = (u32int*)((u32int)thread->m_userStack.addr + thread->m_userStack.size);
		stack--;
		*stack = (u32int)data;
		stack--;
		*stack = 0;
		u32int esp = (u32int)stack, eip = (u32int)entry_point;
		//Setup a false structure for returning from an interrupt :
		//- update data segments to 0x23 = user data segment with RPL=3
		//- mov esp in ebx and eip in ecx
		//- push value for ss : 0x23 (user data seg rpl3)
		//- push value for esp
		//- push flags
		//- update flags, set IF = 1 (interrupts flag)
		//- push value for cs : 0x1B = user code segment with RPL=3
		//- push eip
		//- return from fake interrupt
		asm volatile("				\
				mov $0x23, %%ax;	\
				mov %%ax, %%ds;		\
				mov %%ax, %%es;		\
				mov %%ax, %%fs;		\
				mov %%ax, %%gs;		\
									\
				mov %0, %%ebx;		\
				mov %1, %%ecx;		\
				pushl $0x23;		\
				pushl %%ebx;		\
				pushf;				\
				pop %%eax;			\
				or $0x200, %%eax;	\
				push %%eax;			\
				pushl $0x1B;		\
				push %%ecx;			\
				iret;				\
				" : : "r"(esp), "r"(eip));
	}
}

Thread::Thread() : Ressource(THIF_OBJTYPE, m_callTable) {	//Private constructor, does nothing
	m_xchgspace = 0;
}

Thread::Thread(thread_entry_t entry_point, void* data, bool iskernel) : Ressource(THIF_OBJTYPE, m_callTable) {
	if (iskernel) {
		setup(Task::getKernelProcess(), entry_point, data, true);
	} else {
		setup(Task::currProcess(), entry_point, data, false);
	}
}

Thread::Thread(Process* process, thread_entry_t entry_point, void* data) : Ressource(THIF_OBJTYPE, m_callTable) {
	setup(process, entry_point, data, false);
}

Thread::~Thread() {
	Task::unregisterThread(this);
	Mem::free(m_kernelStack.addr);
	m_process->getPagedir()->switchTo();
	if (m_userStack.addr != 0) {
		m_process->heap().free(m_userStack.addr);
	}
	if (m_xchgspace != 0) {
		m_process->heap().free(m_xchgspace);
	}
	//Don't unregister thread in process, it has probably already been done
}

void Thread::setup(Process* process, thread_entry_t entry_point, void* data, bool isKernel) {
	m_xchgspace = 0;
	m_isKernel = isKernel;
	m_process = process;
	m_kernelStack.addr = Mem::alloc(STACKSIZE);
	m_kernelStack.size = STACKSIZE;

	if (m_isKernel) {
		m_userStack.size = 0;
		m_userStack.addr = 0;
	} else {
		m_userStack.addr = m_process->heap().alloc(STACKSIZE);
		m_userStack.size = STACKSIZE;
	}
	u32int* stack = (u32int*)((u32int)(m_kernelStack.addr) + m_kernelStack.size);

	//Pass function parameters for runThread()
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

	VirtualTerminal &vt = *(m_process->m_outVT);

	vt << "\nUnhandled exception " << (s32int)no << " at " << (u32int)regs.cs << ":" <<
		(u32int)regs.eip << "\n:: " << exceptions[no];
	if (m_isKernel) PANIC_DUMP("Exception in kernel thread", &regs);

	if (no == 14) {	//Page fault
		int present = !(regs.err_code & 0x1);
		int rw = regs.err_code & 0x2;
		int us = regs.err_code & 0x4;
		int rsvd = regs.err_code & 0x8;
		u32int faddr;
		asm volatile("mov %%cr2, %0" : "=r"(faddr));
		vt << "\n   ";
		if (present) vt << "Present ";
		if (rw) vt << "R/W ";
		if (us) vt << "User ";
		if (rsvd) vt << "Rsvd ";
		vt << "At:" << (u32int)faddr;

		vt << "\nThread finishing.\n";
		Task::currentThreadExits(E_PAGEFAULT);	//Calling this will setup a new stack
		return;
	}
	vt << "\nThread finishing.\n";
	Task::currentThreadExits(E_UNHANDLED_EXCEPTION);
}

void Thread::setState(u32int esp, u32int ebp, u32int eip) {
	m_esp = esp;
	m_ebp = ebp;
	m_eip = eip;
}

void Thread::setKernelStack() {
	GDT::tss_entry.esp0 = (u32int)(m_kernelStack.addr) + m_kernelStack.size;
}

u32int Thread::getEsp() { return m_esp; }
u32int Thread::getEbp() { return m_ebp; }
u32int Thread::getEip() { return m_eip; }

Process* Thread::getProcess() { return m_process; }

void* Thread::mkXchgSpace(u32int sz) {
	if (m_xchgspace != 0) m_process->heap().free(m_xchgspace);
	m_xchgspace = m_process->heap().alloc(sz);
	return m_xchgspace;
}

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
	if (m_process->getState() != P_RUNNING) return false;
	if (m_state == T_RUNNING) return true;
	if (m_state == T_SLEEPING and Time::time() >= waitfor.m_time) {
		m_state = T_RUNNING;
		return true;
	}
	return false;
}

u32int Thread::sleepSC(u32int msecs) {
	if (this != Task::currThread()) return 1;
	sleep(msecs);
	return 0;
}

u32int Thread::finishSC(u32int errcode) {
	if (this != Task::currThread()) return 1;
	Task::currentThreadExits(errcode);
	return 0;
}


