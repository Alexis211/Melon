#include <App/ShellApp.proto.h>
#include <Binding/Sys.ns.h>

class reboot : public ShellApp {
	public:
	reboot() : ShellApp("reboot", "Reboot the system") {}
	int run() {
		outvt << "Reboot system...\n";
		Sys::reboot();
	}
};

APP(reboot);
