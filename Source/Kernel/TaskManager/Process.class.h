#ifndef DEF_PROCESS_CLASS_H
#define DEF_PROCESS_CLASS_H

#include <Library/String.class.h>
#include <Library/Vector.class.h>
#include <MemoryManager/PageDirectory.class.h>
#include <VTManager/VirtualTerminal.class.h>

#define P_ZOMBIE 0
#define P_RUNNING 1
#define P_FINISHED 2

#define E_PAGEFAULT 0x0FFFFF00
#define E_ABORTED 0x0FFFFF01
#define E_UNHANDLED_EXCEPTION 0x0FFFFF02

#define STACKSIZE 4096	//Can change

class Thread;

class Process {
	friend class Thread;

	private:
	Process(); //Creates an empty process, used by creatKernel()

	u32int m_pid;
	String m_cmdline;
	s32int m_retval;	//Can be either a standard return value or an E_* (see #defines above)
	u8int m_state; 	//Is one of P_* defined above
	PageDirectory* m_pagedir;
	u32int m_uid;	//User ID
	u32int m_stacksstart;
	VirtualTerminal *m_vt;

	Vector<Thread*> m_threads;
	
	public:
	static Process* createKernel(String cmdline, VirtualTerminal *vt);	//Also creates a Thread for what's curently happening
	Process(String cmdline, u32int uid);
	~Process();

	u32int stackAlloc();	//Allocates pages for STACKSIZE bytes at end of app memory (just before 0xC0000000)
	void exit();	//Exits properly process by killing all threads
	void registerThread(Thread* t);	//Called when a thread starts
	void threadFinishes(Thread* thread, u32int retval); //Called when a thread finishes

	PageDirectory* getPagedir();

	VirtualTerminal* getVirtualTerminal();
	void setVirtualTerminal(VirtualTerminal* vt);

};

#endif
