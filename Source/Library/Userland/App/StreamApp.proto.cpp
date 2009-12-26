#include "StreamApp.proto.h"

#include <FileStream.class.h>

StreamApp::StreamApp(const String& name, const String& desc)
	: ShellApp(name, desc) {
	addFlag("o", "output", "Set the output to a file instead of the text output", FT_STR, "");
	addFlag("e", "encoding", "Set the encoding for files (input and output)", FT_STR, "utf8");
	addFlag("a", "append", "When writing to a file, append instead of truncating", FT_BOOL, "");
}

StreamApp::~StreamApp() {
}

void StreamApp::init() {
	ShellApp::init();
	
	u8int encoding = UE_UTF8;
	if (sFlag("encoding") == "utf8") encoding = UE_UTF8;
	if (sFlag("encoding") == "utf16be") encoding = UE_UTF16_BE;
	if (sFlag("encoding") == "utf16le") encoding = UE_UTF16_LE;
	if (sFlag("encoding") == "utf32be") encoding = UE_UTF32_BE;
	if (sFlag("encoding") == "utf32le") encoding = UE_UTF32_LE;

	if (sFlag("output") == "") {
		out = &outvt;
	} else {
		out = new FileOStream(sFlag("output"), (bFlag("append") ? FM_APPEND : FM_TRUNCATE), encoding, FS::cwdNode());
	}

	if (args.size() == 0) {
		in = &invt;
	} else {
		FileIStream *f = new FileIStream(encoding, FS::cwdNode());
		for (u32int i = 0; i < args.size(); i++) {
			FSNode n = FS::find(args[i], FS::cwdNode());
			if (!n.valid()) {
				outvt << "File does not exist : " << args[i] << ENDL;
				exit(-1);
			} else if (n.type() != NT_FILE) {
				outvt << "Not a file : " << args[i] << ENDL;
				exit(-1);
			}
			f->appendFile(args[i]);
		}
		in = f;
	}
}
