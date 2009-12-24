#include <App/StreamApp.proto.h>

class rot13 : public StreamApp {
	public:
	rot13() : StreamApp("cat", "Concatenate some input files") {
		addFlag("r", "rot13", "Apply a ROT13 encryption to the output", FT_BOOL, "");
	}
	int run();
};

APP(rot13);

int rot13::run() {
	while (!in->eof()) {
		String s = in->get();
		if (in->eof() && s.empty()) break;
		if (bFlag("rot13")) {
			for (u32int i = 0; i < s.size(); i++) {
				WChar &c = s[i];
				if (c >= WChar('A') and c <= WChar('Z')) {
					c += 13;
					if (c > WChar('Z')) c -= 26;
				}
				if (c >= WChar('a') and c <= WChar('z')) {
					c += 13;
					if (c > WChar('z')) c -= 26;
				}
			}
		}
		*out << s << ENDL;
	}
	return 0;
}

