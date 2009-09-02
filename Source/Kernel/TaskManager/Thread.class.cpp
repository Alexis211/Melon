#include "Thread.class.h"
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <DeviceManager/Time.ns.h>

void runThread(Thread* thread, u32int (*entry_point)()) {
	asm volatile("sti");
	u32int ret = entry_point();	//Run !
	asm volatile("mov %0, %%eax; int $66;" : : "r"(ret));	//Syscall for thread ending
}

Thread::Thread() {	//Private constructor, does nothing
}

Thread::Thread(u32int (*entry_point)(), bool iskernel) {
	if (iskernel) {
		m_isKernel = true;
		u32int tmp;
		m_kernelStackFrame = (u32int)PageAlloc::alloc(&tmp);
		m_process = Task::getKernelProcess();
		setup(entry_point, m_kernelStackFrame + STACKSIZE);
	} else {
		m_isKernel = false;
		m_process = Task::currentProcess;
		setup(entry_point, m_process->stackAlloc() + STACKSIZE);
	}
}

Thread::Thread(Process* process, u32int (*entry_point)()) {
	m_isKernel = false;
	m_process = process;
	setup(entry_point, m_process->stackAlloc() + STACKSIZE);
}

Thread::~Thread() {
	if (Task::currentThread == this) Task::currentThread = (Thread*)0xFFFFFFFF;	//Signal that current thread is invalid
	if (m_isKernel)
		PageAlloc::free((void*)m_kernelStackFrame);
	Task::unregisterThread(this);
	//Don't unregister thread in process, it has probably already been done
}

void Thread::setup(u32int (*entry_point)(), u32int esp) {
	//Pass function parameters for runThread()
	u32int *stack = (u32int*)esp;
	stack--;
	*stack = (u32int)entry_point;	//Push entry point (function parameter)
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
	if (!m_isKernel and !m_isRunningAnInterrupt) return;

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
