#include "Shell.class.h"
#include <TextFile.class.h>
#include <Binding/Process.class.h>

void Shell::ls(Vector<String>& args) {
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
			if (((p >> i) & 1) == 0) perm[8 - i] = "-";
		}
		if (n.type() == NT_FILE) {
			outvt << " FILE " << perm << " " << n.getName() << FLUSH;
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " bytes.\n";
		} else if (n.type() == NT_DIRECTORY) {
			outvt << " DIR  " << perm << " " << n.getName() << "/" << FLUSH;
			outvt.setCsrCol(30);
			outvt << (s32int)n.getLength() << " items.\n";
		}
	}
}

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

void Shell::cat(Vector<String>& args) {
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
		Process p = Process::run(args[1]);
		if (p.valid()) {
			p.setInVT(invt);
			p.setOutVT(outvt);
			for (u32int i = 2; i < args.size(); i++) {
				p.pushArg(args[i]);
			}
			p.start();
			s32int v = p.wait();
			outvt << "Return value : " << (s64int)v << "\n";
		} else {
			outvt << "Error while launching process.\n";
		}
	}
}
