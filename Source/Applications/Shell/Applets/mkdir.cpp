#include <App/ShellApp.proto.h>
#include <Binding/FSNode.class.h>

class mkdir : public ShellApp {
	public:
	mkdir() : ShellApp("mkdir", "Create a/some directory/ies") {}
	int run() {
		if (args.size() == 0) outvt << "Usage: mkdir <name> [<name> ...]\n";
		for (u32int i = 0; i < args.size(); i++) {
			if (!FS::mkdir(args[i], FS::cwdNode()).valid()) {
				outvt << "Error while creating directory " << args[i] << "\n";
			}
		}
	}
};

APP(mkdir);
