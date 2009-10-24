#ifndef DEF_PROCESS_CLASS_H
#define DEF_PROCESS_CLASS_H

#include <String.class.h>
#include <Vector.class.h>
#include <SimpleList.class.h>
#include <MemoryManager/PageDirectory.class.h>
#include <Heap.class.h>
#include <VTManager/VirtualTerminal.proto.h>
#include <VFS/File.class.h>
class DirectoryNode;

#include <SyscallManager/Ressource.class.h>

#define P_ZOMBIE 0
#define P_RUNNING 1
#define P_STARTING 2
#define P_FINISHED 3

#define E_PAGEFAULT 0x0FFFFF00
#define E_EXIT 0x0FFFFF01
#define E_UNHANDLED_EXCEPTION 0x0FFFFF02

#define STACKSIZE 4096	//Could change

#define USERHEAPINITSIZE 0x00010000	//Heap initially is 64k, but can grow
#define USERHEAPSTART 0xB7000000	//Heap is at 0xB7000000, 128Mo before kernel space.

class Thread;
class File;

class Process : public Ressource {
	friend class Thread;

	private:
	Process(); //Creates an empty process, used by creatKernel()

	u32int m_pid;	//Process IDentifier
	u32int m_ppid;	//Parent PID
	Vector<String> m_arguments;
	s32int m_retval;	//Can be either a standard return value or an E_* (see #defines above)
	u8int m_state; 	//Is one of P_* defined above
	PageDirectory* m_pagedir;
	u32int m_uid;	//User ID
	VirtualTerminal *m_inVT, *m_outVT;
	DirectoryNode *m_cwd;

	bool m_autodelete;

	Heap *m_userHeap;

	Vector<Thread*> m_threads;
	SimpleList<File*> *m_fileDescriptors;

	//System calls
	static call_t m_callTable[];
	u32int exitSC();
	u32int argcSC();
	u32int argvSC(u32int);
	u32int allocPageSC(u32int);
	u32int freePageSC(u32int);
	u32int startSC();	//Permits parent process to start run of process
	u32int autoDeleteSC(u32int);	//If true, process will auto-delete when it finishes. Else, it must be deleted by parent, while waiting for it.
	u32int pushArgSC(u32int);
	u32int setOutVTSC(u32int);
	u32int setInVTSC(u32int);
	bool accessible();
	
	public:
	static u32int scall(u8int, u32int, u32int, u32int, u32int);

	static Process* createKernel(String cmdline, VirtualTerminal *vt);	//Also creates a Thread for what's curently happening
	static Process* run(String filename, u32int uid);
	Process(String binfile, u32int uid);
	~Process();

	Heap& heap() { return *m_userHeap; }

	void start();	//Starts thread execution - sets m_state to P_RUNNING if == P_STARTING
	void pushArg(const String& arg);
	void exit();	//Exits properly process by killing all threads and deleting file descriptors
	void registerThread(Thread* t);	//Called when a thread starts
	void threadFinishes(Thread* thread, u32int retval); //Called when a thread finishes

	void registerFileDescriptor(File* fd);
	void unregisterFileDescriptor(File* fd);

	PageDirectory* getPagedir();
	u32int getUid() { return m_uid; }
	u32int getPid() { return m_pid; }
	u32int getPpid() { return m_ppid; }

	void setCwd(DirectoryNode *cwd) { m_cwd = cwd; }
	DirectoryNode *getCwd() { return m_cwd; }

	VirtualTerminal* getInVT();
	VirtualTerminal* getOutVT();
	void setInVT(VirtualTerminal* vt);
	void setOutVT(VirtualTerminal* vt);
	u32int getState() { return m_state; }
};

#endif
