#ifndef DEF_PROCESS_CLASS_H
#define DEF_PROCESS_CLASS_H

#include <Library/String.class.h>
#include <Library/Vector.class.h>
#include <Library/SimpleList.class.h>
#include <MemoryManager/PageDirectory.class.h>
#include <MemoryManager/Heap.class.h>
#include <VTManager/VirtualTerminal.proto.h>

#define P_ZOMBIE 0
#define P_RUNNING 1
#define P_FINISHED 2

#define E_PAGEFAULT 0x0FFFFF00
#define E_ABORTED 0x0FFFFF01
#define E_UNHANDLED_EXCEPTION 0x0FFFFF02

#define STACKSIZE 4096	//Could change

#define USERHEAPINITSIZE 0x00010000	//Heap initially is 64k, but can grow
#define USERHEAPSTART 0xB7000000	//Heap is at 0xB7000000, 128Mo before kernel space.

class Thread;
class File;

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
	VirtualTerminal *m_vt;

	Heap *m_userHeap;

	Vector<Thread*> m_threads;
	SimpleList<File*> *m_fileDescriptors;
	
	public:
	static Process* createKernel(String cmdline, VirtualTerminal *vt);	//Also creates a Thread for what's curently happening
	Process(String cmdline, u32int uid);
	~Process();

	Heap& heap() { return *m_userHeap; }

	void exit();	//Exits properly process by killing all threads and deleting file descriptors
	void registerThread(Thread* t);	//Called when a thread starts
	void threadFinishes(Thread* thread, u32int retval); //Called when a thread finishes

	void registerFileDescriptor(File* fd);
	void unregisterFileDescriptor(File* fd);

	PageDirectory* getPagedir();

	VirtualTerminal* getVirtualTerminal();
	void setVirtualTerminal(VirtualTerminal* vt);
	u32int getState() { return m_state; }

};

#endif
