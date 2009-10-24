#include <Binding/VirtualTerminal.class.h>
#include <Binding/FSNode.class.h>
#include <String.class.h>

namespace Shell {
	u32int run();

	extern FSNode cwd;

	extern void ls(Vector<String>& args);
	extern void cd(Vector<String>& args);
	extern void pwd(Vector<String>& args);
	extern void rm(Vector<String>& args);
	extern void mkdir(Vector<String>& args);
	extern void cat(Vector<String>& args);
	extern void wf(Vector<String>& args);
	extern void run(Vector<String>& args);
}
