#include "Shell.class.h"
#include <Binding/Sys.ns.h>
#include <Binding/Process.class.h>

APP(Shell);

Shell::Shell() : ShellApp("Shell.app", "The Melon command line interpreter"), cwd(FS::cwdNode()) {
}

void Shell::setupVars() {
	shell_var_t tmp;
	tmp.readonly = true;
	tmp.value = String::number(pr.getUid());
	m_vars.set("UID", tmp);
	tmp.value = String::number(pr.getPid());
	m_vars.set("PID", tmp);
	tmp.value = "/";
	m_vars.set("CWD", tmp);
}

int Shell::run() {
	struct {	//Command list
		String name;
		void (Shell::*cmd)(Vector<String>&);
	} commands[] = {
		{"cd",		&Shell::cd},
		{"pwd",		&Shell::pwd},
		{"rm",		&Shell::rm},
		{"mkdir",	&Shell::mkdir},
		{"wf",		&Shell::wf},
		{"run",		&Shell::run},
		{"",		0}
	};

	String dn = FS::dirname(pr.argv(0));
	FSNode shellDir = (dn.empty() ? FS::cwdNode() : FS::find(dn, FS::cwdNode()));
	String appletsDir = FS::find("Applets", shellDir).path() + "/";

	setupVars();

	cwd = FS::cwdNode();
	while (1) {
		outvt << MVT::setfgcolor(PROMPTS_COLOR) << "{" << MVT::setfgcolor(PROMPTV_COLOR) << cwd.getName() << 
			MVT::setfgcolor(PROMPTS_COLOR) << "}: " << MVT::setfgcolor(ENTRY_COLOR) << FLUSH;
		m_vars["CWD"].value = cwd.path();
		String s = invt.get();
		outvt << MVT::setfgcolor(NORMAL_COLOR) << FLUSH;
		if (s.contains(EOF)) return 0;
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

		for (u32int i = 0; i < cmd.size(); i++) {
			if (cmd[i][0] == WChar("$")) {
				String k = cmd[i].substr(1);
				if (m_vars.has(k)) {
					cmd[i] = m_vars[k].value;
				} else {
					outvt << "Unknown variable : " << k << ENDL;
				}
			}
		}

		//Run command
		if (cmd[0] == "exit") {
			if (cmd.size() == 1) return 0;
			return cmd[1].toInt();
		} else if (cmd[0] == "help") {
			Vector<String> args;
			args.push("cat");
			args.push(shellDir.path());
			if (args.back() != "/") args.back() += "/";
			args.back() += "Help.txt";
			if (!appRun(appletsDir + "cat", args)) outvt << "Could not find cat command to display help file\n";
		} else if (cmd[0] == "echo") {
			for (u32int i = 1; i < cmd.size(); i++) {
				if (i > 1) outvt << " ";
				outvt << cmd[i];
			}
			outvt << ENDL;
		} else {
			u32int i = 0;
			bool found = false;
			while (!commands[i].name.empty()) {
				if (commands[i].name == cmd[0]) {
					found = true;
					if (commands[i].cmd == 0) {
						outvt << "Not implemented yet.\n";
					} else {
						(this->*(commands[i].cmd))(cmd);
					}
					break;
				}
				i++;
			}
			if (!found) {
				if (!appRun(appletsDir + cmd[0], cmd))
					outvt << "Unknown command : " << cmd[0] << "\n";
			}
		}
	}
}



bool Shell::appRun(const String& app, Vector<String>& args) {
	Process p = Process::run(app);
	if (p.valid()) {
		p.setInVT(invt);
		p.setOutVT(outvt);
		for (u32int i = 1; i < args.size(); i++) {
			p.pushArg(args[i]);
		}
		p.start();
		s32int v = p.wait();
		outvt << "Return value : " << (s64int)v << ENDL;
		return true;
	}
	return false;
}
