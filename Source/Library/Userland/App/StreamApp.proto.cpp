#include "StreamApp.proto.h"

#include <FileStream.class.h>

StreamApp::StreamApp(const String& name, const String& desc)
	: ShellApp(name, desc) {
	addFlag("o", "output", "Set the output to a file instead of the text output", FT_STR, "");
	addFlag("e", "encoding", "Set the encoding for files (input and output)", FT_STR, "utf8");
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
		out = new FileOStream(sFlag("output"), FM_TRUNCATE, encoding, FS::cwdNode());
	}

	if (args.size() == 0) {
		in = &invt;
	} else {
		FileIStream *f = new FileIStream(encoding, FS::cwdNode());
		for (u32int i = 0; i < args.size(); i++) {
			f->appendFile(args[i]);
		}
		in = f;
	}
}