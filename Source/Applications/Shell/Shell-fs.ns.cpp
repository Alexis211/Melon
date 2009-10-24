#include "Shell.ns.h"
#include <TextFile.class.h>

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
	if (!d.valid()) return;
	for (u32int i = 0; i < d.getLength(); i++) {
		FSNode n = d.getChild(i);
		if (!n.valid()) {
			outvt << " [inacessible file]\n";	//This is a file we are not supposed to be able to read
			continue;
		}
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

void mkdir(Vector<String>& args) {
	if (args.size() == 1) outvt << "No directory to create.\n";
	for (u32int i = 1; i < args.size(); i++) {
		if (!FS::mkdir(args[i], cwd).valid()) {
			outvt << "Error while creating directory " << args[i] << "\n";
		}
	}
}

void cat(Vector<String>& args) {
	if (args.size() == 1) outvt << "Meow.\n";
	for (u32int i = 1; i < args.size(); i++) {
		TextFile f(args[i], FM_READ, cwd);
		if (f.valid() && f.validOpened()) {
			while (!f.eof()) {
				outvt << f.readLine() << "\n";
			}
			f.close();
		} else {
			outvt << "Error while reading from file " << args[i] << "\n";
		}
	}
}

void wf(Vector<String>& args) {
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

}
