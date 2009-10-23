#include "Shell.ns.h"
#include <Binding/Sys.ns.h>

namespace Shell {

FSNode node(0);

u32int run() {
	node = FSNode::getRoot();
	while (1) {
		outvt << node.getName() << " : ";
		String s = invt.readLine();
		while (s[0] == WChar(" ") or s[0] == WChar("\t")) {
			s = s.substr(1, s.size() - 1);
		}
		if (s[0] == WChar("#")) continue;
		if (s.empty()) continue;

		//Parse string
		Vector<String> cmd;
		cmd.push(String());
		bool inQuote = false;
		for (u32int i = 0; i < s.size(); i++) {
			if (s[i] == WChar("'")) {
				inQuote = !inQuote;
			} else if (s[i] == WChar("\\")) {
				i++;
				cmd.back() += s[i];
			} else if (s[i] == WChar(" ") and !inQuote) {
				cmd.push(String());
			} else {
				cmd.back() += s[i];
			}
		}

		//Run command
		if (cmd[0] == "exit") {
			if (cmd.size() == 1) return 0;
			return cmd[1].toInt();
		} else if (cmd[0] == "halt") {
			Sys::halt();
			outvt << "Something went wrong.\n";
		} else if (cmd[0] == "reboot") {
			Sys::reboot();
			outvt << "Something went wrong.\n";
		} else {
		}
	}
}

}


