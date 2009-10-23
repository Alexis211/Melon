#include "Shell.ns.h"

namespace Shell {

void ls(Vector<String>& args) {
	FSNode d = cwd;
	if (args.size() == 2) {
		FSNode n = FS::find(args[1], cwd);
		d = FSNode(0);
		if (!n.valid())
			outvt << "No such directory : " << args[1] << "\n";
		else if (n.type() != NT_DIRECTORY)
			outvt << "Not a directory : " << args[1] << "\n";
		else
			d = n;
	}	
	if (d.valid()) outvt << "Contents of directory " << d.path() << " :\n";
	for (u32int i = 0; i < d.getLength(); i++) {
		FSNode n = d.getChild(i);
		String perm = "rwxrwxrwx";
		u32int p = n.getPerm();
		for (u32int i = 0; i < 9; i++) {
			if (((p >> i) & 1) == 0) perm[9 - i] = "-";
		}
		if (n.type() == NT_FILE) {
			outvt << " FILE " << perm << " " << n.getName();
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " bytes.\n";
		} else if (n.type() == NT_DIRECTORY) {
			outvt << " DIR  " << perm << " " << n.getName() << "/";
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " items.\n";
		}
	}
}

void cd(Vector<String>& args) {
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

void pwd(Vector<String>& args) {
	outvt << "Current directory : " << cwd.path() << "\n";
}

void rm(Vector<String>& args) {
	if (args.size() == 1) outvt << "No file to remove.\n";
	for (u32int i = 1; i < args.size(); i++) {
		if (!FS::find(args[i], cwd).remove()) {
			outvt << "Error while removing file " << args[i] << "\n";
		}
	}
}

}
