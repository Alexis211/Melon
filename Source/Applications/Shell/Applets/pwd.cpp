#include <App/ShellApp.proto.h>
#include <Binding/FSNode.class.h>

class pwd : public ShellApp {
	public:
	pwd() : ShellApp("pwd", "Shows the current directory") {
		addFlag("f", "frugal", "Do not show 'Current directory: ' prefix", FT_BOOL, "");
	}
	int run() {
		if (!bFlag("frugal")) outvt << "Current directory: ";
		outvt << FS::cwdNode().path() << ENDL;
	}
};

APP(pwd);
