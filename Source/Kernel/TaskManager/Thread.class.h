#ifndef DEF_THREAD_CLASS_H
#define DEF_THREAD_CLASS_H

#include <TaskManager/Process.class.h>
#include <SyscallManager/IDT.ns.h>

#define T_ZOMBIE 0
#define T_RUNNING 1
#define T_SLEEPING 2
#define T_IRQWAIT 3	

typedef u32int(*thread_entry_t)(void*);

class Thread : public Ressource {
	friend class Process;	//This might be useful

	protected:
	static void run(Thread* thread, void* data, thread_entry_t entry_point);

	Thread();	//Creates a thread without initializing anything. Used by Process::createKernel();

	Process *m_process;	//Associated process
	u32int m_esp, m_ebp, m_eip;
	u8int m_state;	//Is one of T_* defined above

	void* m_xchgspace;

	union {		//What the thread might be waiting for
		u32int m_time;
		u8int m_irq;		//An IRQ number
	} waitfor;
	
	bool m_isKernel;	//Says if stack is in kernel pagedir, and if thread should run in ring 0
	struct {
		void* addr;
		u32int size;
	} m_userStack, m_kernelStack;

	void setup(Process* process, thread_entry_t entry_point, void* data, bool isKernel);

	//Syscalls
	static call_t m_callTable[];
	u32int sleepSC(u32int msecs);
	u32int finishSC(u32int errcode);
	bool accessible();

	public:
	static u32int scall(u8int, u32int, u32int, u32int, u32int);

	Thread(thread_entry_t entry_point, void* data, bool iskernel = false);	//Assumes process is current process, or is kprocess if isk
	Thread(Process* process, thread_entry_t entry_point, void* data);
	~Thread();
	void finish(u32int errcode);	//Called by run() when thread returns, and by exception handler. Can also be called by the thread itself
	virtual void handleException(registers_t *regs, int no);

	void setState(u32int esp, u32int ebp, u32int eip);
	void setKernelStack();
	u32int getEsp();
	u32int getEbp();
	u32int getEip();
	Process* getProcess();

	void* mkXchgSpace(u32int sz);

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
