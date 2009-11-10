#include <iostream>
#include <fstream>

#define BMP_CHARWIDTH 9

#define CHARHEIGHT 16

#define CHARACTERS 256

using namespace std;

int main() {
	ifstream f("ConsoleFont.raw", ios::in | ios::binary);

	bool Chars[CHARACTERS][8][CHARHEIGHT];

	for (int y = 0; y < CHARHEIGHT; y++) {
		for (int x = 0; x < CHARACTERS * BMP_CHARWIDTH; x++) {
			char temp; f.read(&temp, 1);
			int cc = x / BMP_CHARWIDTH;
			int xx = x % BMP_CHARWIDTH;
			if (xx < 8) Chars[cc][xx][y] = (temp != 0);
		}
	}
	f.close();

	ofstream cdfont("ConsoleFont.cd", ios::out);
	cdfont << "char consoleFont[" << CHARACTERS << "][" << CHARHEIGHT << "] = {" << endl;

	for (int i = 0; i < CHARACTERS; i++) {
		cdfont << hex << "/* 0x" << (i < 16 ? "0" : "") << i << " */\t{";
		for (int y = 0; y < CHARHEIGHT; y++) {
			int n = 0;
			for (int x = 0; x < 8; x++) {
				n = n << 1;
				n += (Chars[i][x][y] ? 1 : 0);
			}
			cdfont << (y == 0 ? "0x" : ", 0x") << (n < 16 ? "0" : "") << hex << n;
		}
		cdfont << "}," << endl;
	}
	cdfont << "};" << endl;
}
