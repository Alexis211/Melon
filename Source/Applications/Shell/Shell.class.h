#ifndef DEF_SHELL_CLASS_H
#define DEF_SHELL_CLASS_H

#include <Binding/VirtualTerminal.class.h>
#include <Binding/FSNode.class.h>
#include <String.class.h>

#include <App/ShellApp.proto.h>

class Shell : public ShellApp {
	public:
	Shell();

	int run();

	FSNode cwd;

	void ls(Vector<String>& args);
	void cd(Vector<String>& args);
	void pwd(Vector<String>& args);
	void rm(Vector<String>& args);
	void mkdir(Vector<String>& args);
	void cat(Vector<String>& args);
	void wf(Vector<String>& args);
	void run(Vector<String>& args);
};

#endif
