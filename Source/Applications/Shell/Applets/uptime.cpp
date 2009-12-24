#include <App/ShellApp.proto.h>
#include <Binding/Sys.ns.h>

class uptime : public ShellApp {
	public:
	uptime() : ShellApp("uptime", "Get the system's uptime") {
		addFlag("u", "unformatted", "Show raw number of seconds", FT_BOOL, "");
	}
	int run() {
		if (bFlag("unformatted")) {
			outvt << (s64int)Sys::uptime() << ENDL;
		} else {
			u64int secs = Sys::uptime();
			u64int mins = secs / 60;	secs = secs % 60;
			u64int hours = mins / 60;	mins = mins % 60;
			u64int days = hours / 24;	hours = hours % 24;
			outvt << "Uptime: ";
			if (days > 0) outvt << (s64int)days << "d, ";
			if (hours > 0) outvt << (s64int)hours << "h, ";
			if (mins > 0) outvt << (s64int)mins << "m, ";
			outvt << (s64int)secs << "s.\n";
		}
	}
};

APP(uptime);
