#include <App/ShellApp.proto.h>
#include <Binding/Sys.ns.h>

class free : public ShellApp {
	public:
	free() : ShellApp("free", "Get the amount of system free RAM") {}
	int run() {
		outvt << "Free RAM: " << (s64int)Sys::freeRam() << " Kio of " << (s64int)Sys::totalRam() << " Kio\n"; 
	}
};

APP(free);
