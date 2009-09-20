/*  #define THIS_IS_NOT_MELON
#include <cstring>
#include <Library/WChar.class.cpp>
#include "fr.cxd" */
#include "Keymap.h"
#include <fstream>
#include <string>

using namespace std;

int main() {
	melon_keymap_t km;
	memcpy(km.normal, keymap_normal, 128 * sizeof(WChar));
	memcpy(km.shift, keymap_shift, 128 * sizeof(WChar));
	memcpy(km.caps, keymap_caps, 128 * sizeof(WChar));
	memcpy(km.altgr, keymap_altgr, 128 * sizeof(WChar));
	memcpy(km.shiftaltgr, keymap_shiftaltgr, 128 * sizeof(WChar));

	string a = LANG;
	a += ".mkm";

	fstream output(a.c_str(), ios::out | ios::binary);
	output.write((char*)&km, sizeof(km));
	output.close();

	return 0;
}
