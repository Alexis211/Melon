#include <App/ShellApp.proto.h>
#include <Binding/Sys.ns.h>

class halt : public ShellApp {
	public:
	halt() : ShellApp("halt", "Halt the system") {}
	int run() {
		outvt << "Halting system...\n";
		Sys::halt();
	}
};

APP(halt);
