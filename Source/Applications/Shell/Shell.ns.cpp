#include "Shell.ns.h"
#include <Binding/Sys.ns.h>
#include <Binding/Process.class.h>

namespace Shell {

FSNode cwd(0);


u32int run() {
	struct {	//Command list
		String name;
		void (*cmd)(Vector<String>&);
	} commands[] = {
		{"ls",		ls},
		{"cd",		cd},
		{"pwd",		pwd},
		{"rm",		rm},
		{"mkdir",	mkdir},
		{"cat",		cat},
		{"wf",		wf},
		{"run",		run},
		{"",		0}
	};

	cwd = FS::cwdNode();
	while (1) {
		outvt << "{" << cwd.getName() << "}: ";
		String s = invt.readLine();
		if (s.empty()) continue;
		while (s[0] == WChar(" ") or s[0] == WChar("\t")) {
			s = s.substr(1, s.size() - 1);
		}
		if (s.empty()) continue;
		if (s[0] == WChar("#")) continue;

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
		} else if (cmd[0] == "uptime") {
			outvt << "Uptime : " << (s64int)Sys::uptime() << "s\n";
		} else if (cmd[0] == "free") {
			outvt << "Free RAM : " << (s64int)Sys::freeRam() << " Kio of " << (s64int)Sys::totalRam() << " Kio\n"; 
		} else if (cmd[0] == "uid") {
			outvt << "User ID : " << (s64int)(Process::get().getUid()) << "\n";
		} else if (cmd[0] == "help") {
			while (cmd.size() > 1) cmd.pop();
			cmd.push("/Applications/Shell/Help.txt");
			cat(cmd);
		} else {
			u32int i = 0;
			bool found = false;
			while (!commands[i].name.empty()) {
				if (commands[i].name == cmd[0]) {
					found = true;
					if (commands[i].cmd == 0) {
						outvt << "Not implemented yet.\n";
					} else {
						commands[i].cmd(cmd);
					}
					break;
				}
				i++;
			}
			if (!found) outvt << "Unknown command : " << cmd[0] << "\n";
		}
	}
}

}


