#ifndef DEF_KERNELSHELL_CLASS_H
#define DEF_KERNELSHELL_CLASS_H

#include <VTManager/VirtualTerminal.proto.h>
#include <VFS/DirectoryNode.class.h>
#include <TaskManager/Thread.class.h>
#include <Vector.class.h>

class KernelShell {
	friend u32int shellRun(void* ks);

	private:
	VirtualTerminal *m_vt;
	DirectoryNode *m_cwd;
	Thread* m_thread;
	static u32int m_instances;

	~KernelShell();
	u32int run();

	//in KernelShell-fs
	void ls(Vector<String>& args);
	void cd(Vector<String>& args);
	void pwd(Vector<String>& args);
	void cat(Vector<String>& args);
	void mkdir(Vector<String>& args);
	void rm(Vector<String>& args);
	void wf(Vector<String>& args);
	void run(Vector<String>& args);
	void hexdump(Vector<String>& args);

	//in KernelShell-sys
	void devices(Vector<String>& args);
	void loadkeys(Vector<String>& args);
	void free(Vector<String>& args);
	void uptime(Vector<String>& args);
	void part(Vector<String>& args);
	void readblock(Vector<String>& args);
	void mount(Vector<String>& args);
	void unmount(Vector<String>& args);

	void setup(DirectoryNode* cwd, VirtualTerminal *vt);

	public:
	KernelShell(DirectoryNode* cwd, VirtualTerminal *vt);
	KernelShell(DirectoryNode* cwd);
	
	static u32int getInstances() { return m_instances; }
};

#endif
