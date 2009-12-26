#include <App/ShellApp.proto.h>
#include <Binding/FSNode.class.h>

class rm : public ShellApp {
	public:
	rm() : ShellApp("rm", "Remove a/some file(s)/directorie(s)") {}
	int run() {
		if (args.size() == 0) outvt << "Usage : rm <file> [<file> ...]\n";
		for (u32int i = 0; i < args.size(); i++) {
			if (!FS::find(args[i], FS::cwdNode()).remove()) {
				outvt << "Error while removing file " << args[i] << "\n";
			}
		}
	}
};

APP(rm);
