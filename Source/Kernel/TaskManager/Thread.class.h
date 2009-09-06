#ifndef DEF_THREAD_CLASS_H
#define DEF_THREAD_CLASS_H

#include <TaskManager/Process.class.h>
#include <SyscallManager/IDT.ns.h>

#define T_ZOMBIE 0
#define T_RUNNING 1
#define T_SLEEPING 2
#define T_IRQWAIT 3		//This can only happen if process->uid == 0 (root)

class Thread {
	friend class Process;	//This might be useful

	private:
	Thread();	//Creates a thread without initializing anything. Used by Process::createKernel();

	Process *m_process;	//Associated process
	u32int m_esp, m_ebp, m_eip;
	u8int m_state;	//Is one of T_* defined above

	union {		//What the thread might be waiting for
		u32int m_time;
		u8int m_irq;		//An IRQ number
	} waitfor;
	
	bool m_isKernel;	//Says if stack is in kernel pagedir, and if thread should run in ring 0
	u32int m_kernelStackFrame;	//Used for allocating and freeing a frame used as a stack

	void setup(u32int (*entry_point)(), u32int esp);	//Sets up stack, called by both constructors

	public:
	Thread(u32int (*entry_point)(), bool iskernel = false);	//Assumes process is current process, or is kprocess if isk
	Thread(Process* process, u32int (*entry_point)());
	~Thread();
	void finish(u32int errcode);	//Called by run() when thread returns, and by exception handler. Can also be called by the thread itself
	void handleException(registers_t regs, int no);

	void setState(u32int esp, u32int ebp, u32int eip);
	u32int getEsp();
	u32int getEbp();
	u32int getEip();
	Process* getProcess();

	void sleep(u32int msecs);
	void waitIRQ(u8int irq);
	bool runnable();	//Called by scheduler
	inline bool irqHappens(u8int irq) {	//Inline for speed
		if (m_state == T_IRQWAIT and waitfor.m_irq == irq) {
			m_state = T_RUNNING;
			return true;
		}
		return false;
	}
};

#endif
