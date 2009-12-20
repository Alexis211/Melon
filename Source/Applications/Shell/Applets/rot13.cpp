#include <App/StreamApp.proto.h>

class rot13 : public StreamApp {
	public:
	rot13() : StreamApp("rot13", "Cat a file, but ROT13 it") {}
	int run();
};

APP(rot13);

int rot13::run() {
	while (!in->eof()) {
		String s = in->get();
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
		if (in->eof() && s.empty()) break;
		*out << s << ENDL;
	}
	return 0;
}

