#include <App/ShellApp.proto.h>
#include <Binding/FSNode.class.h>

#define DIR_COLOR 3
#define FILE_COLOR 2
#define NORMAL_COLOR 7

class ls : public ShellApp {
	public:
	ls() : ShellApp("ls", "List the content of a directory") {
		addFlag("l", "long", "Use long output", FT_BOOL, "");
	}
	int run();
	void doLs(FSNode n);
};

int ls::run() {
	FSNode cwd = FS::cwdNode();
	if (args.size() == 0) {
		doLs(cwd);
	} else {
		for (u32int i = 0; i < args.size(); i++) {
			FSNode n = FS::find(args[i], cwd);
			if (!n.valid()) {
				outvt << "No such directory : " << args[1] << ENDL;
			} else if (n.type() != NT_DIRECTORY) {
				outvt << "Not a directory : " << args[1] << ENDL;
			} else {
				if (args.size() > 1) outvt << n.path() << ":\n";
				doLs(n);
				if (args.size() > 1 and i != args.size() - 1) outvt << ENDL;
			}
		}
	}
	return 0;
}

void ls::doLs(FSNode n) {
	if (!n.valid()) {
		outvt << "Invalid node...\n";
		return;
	}
	Vector<String> elements;	int maxLength = 0;
	for (u32int i = 0; i < n.getLength(); i++) {
		FSNode c = n.getChild(i);
		if (!n.valid()) {
			outvt << "[inacessible file]\n";
			continue;
		}
		if (bFlag("long")) {
			String perm = "rwxrwxrwx";
			u32int v = c.getPerm();
			for (u32int i = 0; i < 9; i++) {
				if (((v >> i) & 1) == 0) perm[8 - i] = "-";
			}
			if (c.type() == NT_FILE) {
				outvt << " FILE " << perm << " " << c.getName();
				outvt << MVT::setcsrcol(35) << (s32int)c.getLength() << " bytes.\n";
			} else if (c.type() == NT_DIRECTORY) {
				outvt << " DIR  " << perm << " " << c.getName() << "/";
				outvt << MVT::setcsrcol(35) << (s32int)c.getLength() << " items.\n";
			}
		} else {
			String s = c.getName();
			if (c.type() == NT_DIRECTORY) s += "/";
			if (s.size() > maxLength) maxLength = s.size();
			elements.push(s);
		}
	}
	if (!bFlag("long")) {
		int nent;
		bool boxed = outvt.isBoxed();
		if (!boxed) {
			nent = 1;
		} else {
			nent = outvt.width() / (maxLength + 2);
		}
		for (int i = 0, e = 0; i < elements.size(); i++, e++) {
			if (e >= nent) {
				e = 0;
				outvt << ENDL;
			}
			if (boxed) outvt << MVT::setcsrcol(e * (maxLength + 2));
			if (elements[i][elements[i].size() - 1] == WChar("/")) {
				outvt << MVT::setfgcolor(DIR_COLOR);
			} else {
				outvt << MVT::setfgcolor(FILE_COLOR);
			}
			outvt << elements[i] << MVT::setfgcolor(NORMAL_COLOR);
		}
		outvt << ENDL;
	}
}

APP(ls);
