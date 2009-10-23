#include "Shell.ns.h"

namespace Shell {

void ls(Vector<String>& args) {
	FSNode d = cwd;
	if (args.size() == 2) {
		FSNode n = cwd.findFrom(args[1]);
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
		if (n.type() == NT_FILE) {
			outvt << " - FILE\t" << n.getName();
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " bytes.\n";
		} else if (n.type() == NT_DIRECTORY) {
			outvt << " - DIR\t" << n.getName() << "/";
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " items.\n";
		}
	}
}

void cd(Vector<String>& args) {
	if (args.size() != 2) {
		outvt << "Invalid argument count.\n";
	} else {
		FSNode ncwd = cwd.findFrom(args[1]);
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

}
