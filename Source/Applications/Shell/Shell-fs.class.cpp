#include "Shell.class.h"
#include <TextFile.class.h>
#include <Binding/Process.class.h>

void Shell::cd(Vector<String>& args) {
	if (args.size() != 2) {
		outvt << "Invalid argument count.\n";
	} else {
		FSNode ncwd = FS::find(args[1], cwd);
		if (!ncwd.valid()) {
			outvt << "No such directory : " << args[1] << "\n";
		} else if (ncwd.type() == NT_DIRECTORY) {
			ncwd.setCwd();
			cwd = ncwd;
		} else {
			outvt << "Not a directory.\n";
		}
	}
}

void Shell::pwd(Vector<String>& args) {
	outvt << "Current directory : " << cwd.path() << "\n";
}

void Shell::rm(Vector<String>& args) {
	if (args.size() == 1) outvt << "No file to remove.\n";
	for (u32int i = 1; i < args.size(); i++) {
		if (!FS::find(args[i], cwd).remove()) {
			outvt << "Error while removing file " << args[i] << "\n";
		}
	}
}

void Shell::mkdir(Vector<String>& args) {
	if (args.size() == 1) outvt << "No directory to create.\n";
	for (u32int i = 1; i < args.size(); i++) {
		if (!FS::mkdir(args[i], cwd).valid()) {
			outvt << "Error while creating directory " << args[i] << "\n";
		}
	}
}

void Shell::wf(Vector<String>& args) {
	if (args.size() == 1) {
		outvt << "No file to write !\n";
	} else {
		TextFile f(args[1], FM_TRUNCATE, cwd);
		if (f.valid() && f.validOpened()) {
			outvt << "Enter contents for file " << args[1] << " and end your entry with <CR>.<CR>\n";
			String t = invt.readLine();
			while (t != ".") {
				f.write(t, true);
				t = invt.readLine();
			}
			f.close();
		} else {
			outvt << "Error opening file.\n";
		}
	}
}

void Shell::run(Vector<String>& args) {
	if (args.size() == 1) {
		outvt << "Nothing to run...\n";
	} else {
		Vector<String> appargs;
		for (u32int i = 1; i < args.size(); i++) appargs.push(args[i]);
		if (!appRun(args[1], appargs)) {
			outvt << "Error while launching process.\n";
		}
	}
}
