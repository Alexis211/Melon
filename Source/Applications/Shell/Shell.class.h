#ifndef DEF_SHELL_CLASS_H
#define DEF_SHELL_CLASS_H

#include <Binding/VirtualTerminal.class.h>
#include <Binding/FSNode.class.h>
#include <String.class.h>
#include <Map.class.h>

#include <App/ShellApp.proto.h>

#define PROMPTS_COLOR 1
#define PROMPTV_COLOR 9
#define ENTRY_COLOR 6
#define NORMAL_COLOR 7

struct shell_var_t {
	bool readonly;
	String value;
};

class Shell : public ShellApp {
	Map<String, shell_var_t> m_vars;

	void setupVars();

	public:
	Shell();

	int run();

	FSNode cwd;

	bool appRun(const String& name, Vector<String>& args);

	void rm(Vector<String>& args);
	void mkdir(Vector<String>& args);
	void wf(Vector<String>& args);
};

#endif
