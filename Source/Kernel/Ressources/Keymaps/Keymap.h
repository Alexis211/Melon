#include <Library/WChar.class.h>

struct melon_keymap_t {
	WChar normal[128];
	WChar shift[128];
	WChar caps[128];
	WChar altgr[128];
	WChar shiftaltgr[128];
};
