#include "ShellApp.proto.h"
	
ShellApp::ShellApp(String name, String desc)
   	: Application(), invt(VirtualTerminal::getIn()), outvt(VirtualTerminal::getOut()) {
	appName = name, appDesc = desc;
	if (!invt.valid()) exit(1);
	if (!outvt.valid()) exit(2);

	addFlag("h", "help", "Show this help screen");
}

ShellApp::~ShellApp() {
	outvt << END;
}

void ShellApp::init() {
	//Parse flags
	u32int argc = pr.argc();
	for (u32int i = 0; i < argc; i++) {
		String arg = pr.argv(i);
		if (arg.empty()) continue;
		if (arg == "-") {
			i++;
			if (i == argc) {
				args.push("-");
			} else {
				for (; i < argc; i++) args.push(pr.argv(i));
			}
		} else if (arg[0] == WChar("-")) {
			if (arg.substr(0, 5) == "--no-") {
				bool found = false;
				for (u32int i = 0; i < flags.size(); i++) {
					if (flags[i].type == FT_BOOL) {
						if (arg == (String("--no-") += flags[i].lName)) {
							flags[i].boolVal = false;
							found = true;
						}
					}
				}
				if (!found) outvt << "Unknown option : " << arg << ENDL;
			} else if (arg.substr(0, 2) == "--") {
				bool found = false;
				for (u32int i = 0; i < flags.size(); i++) {
					if (flags[i].type == FT_BOOL) {
						if (arg == String("--") + flags[i].lName) {
							flags[i].boolVal = true;
							found = true;
						}
					} else {
						if (arg.substr(2, 1 + flags[i].lName.size()) == flags[i].lName + "=") {
							found = true;
							flags[i].strVal = arg.substr(3 + flags[i].lName.size(), arg.size());
							if (flags[i].type == FT_INT) flags[i].intVal = flags[i].strVal.toInt();
						}
					}
				}
				if (!found) outvt << "Unknown option : " << arg << ENDL;
			} else {
				for (u32int j = 1; j < arg.size(); j++) {
					bool found = false;
					for (u32int k = 0; k < flags.size(); k++) {
						if (flags[k].sName == arg[j]) {
							found = true;
							if (flags[k].type == FT_BOOL) {
								flags[k].boolVal = true;
							} else {
								i++;
								if (i >= argc) {
									outvt << "Missing argument for flag : -" << String(arg[j], 1) << ENDL;
								} else {
									flags[k].strVal = pr.argv(i);
									if (flags[k].type == FT_INT) flags[k].intVal = flags[k].strVal.toInt();
								}
							}
							break;
						}
					}
					if (!found) {
						outvt << "Unknown option : -" << String(arg[j], 1) << ENDL;
						exit(-1);
					}
				}
			}
		} else {
			args.push(arg);
		}
	}
	
	//Eventually show help screen
	if (bFlag("help")) {
		outvt << appName << ": " << appDesc << "\n";
		outvt << "Usage: \t" << appName << " <flags> [-] <arguments>\n\n";
		outvt << "Possible flags :\n";
		for (u32int i = 0; i < flags.size(); i++) {
			outvt << "   --" << flags[i].lName << "\t" << (flags[i].sName != 0 ? "-" : "") << String(flags[i].sName) << "\t";
			if (flags[i].type == FT_STR) outvt << "Default: " << flags[i].strVal << "\n";
			if (flags[i].type == FT_INT) outvt << "Default: " << flags[i].intVal << "\n";
			if (flags[i].type == FT_BOOL) outvt << "Default: " << (flags[i].boolVal && flags[i].lName != "help" ? "on" : "off") << "\n";
			if (!flags[i].desc.empty()) outvt << "\t" << flags[i].desc << "\n";
			outvt << "\n";
		}
		exit(0);
	}
}

//******************** FLAG HANDLING *****************

flag_t *ShellApp::getFlag(String name) {
	for (u32int i = 0; i < flags.size(); i++) {
		if (flags[i].lName == name) return &flags[i];
	}
	return NULL;
}

void ShellApp::addFlag(WChar sName, String lName, String desc, int type, String deflt) {
	if (lName.empty()) return;
	if (getFlag(lName) != false) return;
	flag_t temp;
	temp.sName = sName;
	temp.lName = lName;
	temp.type = type;
	temp.desc = desc;
	if (type == FT_BOOL) {
		temp.boolVal = !deflt.empty();
	} else if (type == FT_STR) {
		temp.strVal = deflt;
	} else if (type == FT_INT) {
		temp.intVal = deflt.toInt();
	}
	flags.push(temp);
}

String ShellApp::sFlag(String name) {
	flag_t* f = getFlag(name);
	if (f == 0) return "";
	if (f->type != FT_STR) return "";
	return f->strVal;
}

int ShellApp::iFlag(String name) {
	flag_t* f = getFlag(name);
	if (f == 0) return 0;
	if (f->type != FT_INT) return 0;
	return f->intVal;
}

bool ShellApp::bFlag(String name) {
	flag_t *f = getFlag(name);
	if (f == 0) return false;
	if (f->type != FT_BOOL) return false;
	return f->boolVal;
}
