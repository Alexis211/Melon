#include "Shell.class.h"
#include <TextFile.class.h>
#include <Binding/Process.class.h>

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

