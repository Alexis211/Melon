#include "Kbd.ns.h"
#include <DeviceManager/Dev.ns.h>
#include <Library/Vector.class.h>
#include <Devices/Keyboard/Keyboard.proto.h>
#include <VTManager/VirtualTerminal.proto.h>

namespace Kbd {

//These are arbitrarily decided codes given to each scancode
u8int ctrlkeys[] = {
/* 0x00 */	0, KBDC_ESCAPE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KBDC_BACKSPACE, KBDC_TAB,
/* 0x10 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KBDC_ENTER, KBDC_LEFTCTRL, 0, 0,
/* 0x20 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KBDC_LEFTSHIFT, 0, 0, 0, 0, 0,
/* 0x30 */	0, 0, 0, 0, 0, 0, KBDC_RIGHTSHIFT, 0, KBDC_ALT, 0, KBDC_CAPSLOCK, KBDC_F1, KBDC_F2, KBDC_F3, KBDC_F4, KBDC_F5,
/* 0x40 */	KBDC_F6, KBDC_F7, KBDC_F8, KBDC_F9, KBDC_F10, KBDC_NUMLOCK, KBDC_SCRLLOCK, KBDC_KPHOME, KBDC_KPUP, KBDC_KPPGUP, 0, KBDC_KPLEFT, KBDC_KP5, KBDC_KPRIGHT, 0, KBDC_KPEND,
/* 0x50 */	KBDC_KPDOWN, KBDC_KPPGDOWN, KBDC_KPINSERT, KBDC_KPDEL, KBDC_SYSREQ, 0, 0, KBDC_F11, KBDC_F12, 0, 0, 0, 0, 0, 0, 0,
/* 0x60 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x70 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x80 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x90 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KBDC_ENTER, KBDC_RIGHTCTRL, 0, 0,
/* 0xA0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xB0 */	0, 0, 0, 0, 0, KBDC_KPSLASH, 0, KBDC_PRTSCN, KBDC_ALTGR, 0, 0, 0, 0, 0, 0, 0,
/* 0xC0 */	0, 0, 0, 0, 0, 0, 0, KBDC_HOME, KBDC_UP, KBDC_PGUP, 0, KBDC_LEFT, 0, KBDC_RIGHT, 0, KBDC_END,
/* 0xD0 */	KBDC_DOWN, KBDC_PGDOWN, KBDC_INSERT, KBDC_DEL, 0, 0, 0, 0, 0, 0, 0, KBDC_LEFTSUP, KBDC_RIGHTSUP, KBDC_MENU, 0, 0,
/* 0xE0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xF0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

WChar *keymapNormal = NULL, *keymapShift = NULL, *keymapCaps = NULL, *keymapAltgr = NULL, *keymapShiftAltgr = NULL;
u8int kbdstatus = 0;
VirtualTerminal *focusedVT = NULL;	//This is the VT that must receive the character

void process(keypress_t kp) {	//This routine sends the information of a keypress to someone
	/*  String r("Key press ");
	String n = String::number(kp.pressed);
	r += n;
	r += ", hascmd=";	
	n = String::number(kp.hascmd);
	r += n;
	r += ", cmd=";
	n = String::number(kp.command);
	r += n;
	r += ", haschar=";
	n = String::number(kp.haschar);
	r += n;
	r += ", char='";
	r += kp.character;
	r += "'";
	DEBUG(r); */
	if (focusedVT != NULL) {
		if (((kp.haschar and kp.character != 0) or (kp.hascmd and kp.command < 100)) and kp.pressed) {
			focusedVT->keyPress(kp);
		}
	}
}

void setFocus(VirtualTerminal* vt) {
	focusedVT = vt;
}

void setKeymap(WChar* kmNormal, WChar* kmShift, WChar* kmCaps, WChar* kmAltgr, WChar* kmShiftAltgr) {
	keymapNormal = kmNormal;
	keymapShift = kmShift;
	keymapCaps = kmCaps;
	keymapAltgr = kmAltgr;
	keymapShiftAltgr = kmShiftAltgr;
}

void updateLeds() {
	Vector<Device*> kbds = Dev::findDevices("keyboard");
	for (u32int i = 0; i < kbds.size(); i++) {
		((Keyboard*)(kbds[i]))->updateLeds(kbdstatus);
	}
}

void keyPress(u8int scancode) {
	keypress_t kp;
	kp.pressed = true;
	kp.modifiers = kbdstatus & 0x0F;
	u8int cmd = ctrlkeys[scancode];
	scancode &= 0x7F;
	if (cmd == 0) cmd = ctrlkeys[scancode];
	if (cmd == 0) {
		kp.haschar = true;
		kp.character = 0;
		if ((kbdstatus & STATUS_ALT) or (kbdstatus & STATUS_CTRL)) {
			kp.hascmd = true;
		}
		if (((kbdstatus & STATUS_SHIFT) != 0) xor ((kbdstatus & STATUS_CAPS) != 0)) {
			if (kbdstatus & STATUS_ALTGR) {
				if (keymapShiftAltgr != NULL) kp.character = keymapShiftAltgr[scancode];
			} else {
				if (keymapCaps != NULL and (kbdstatus & STATUS_CAPS))
					kp.character = keymapCaps[scancode];
				else if (keymapShift != NULL)
					kp.character = keymapShift[scancode];
			}
		} else {
			if (kbdstatus & STATUS_ALTGR) {
				if (keymapAltgr != NULL) kp.character = keymapAltgr[scancode];
			} else {
				if (keymapNormal != NULL) kp.character = keymapNormal[scancode];
			}
		}
	} else if (cmd >= KBDC_KPINSERT and cmd <= KBDC_KPDEL and (kbdstatus & STATUS_NUM)) {
		kp.haschar = true;
		if ((kbdstatus & STATUS_ALT) or (kbdstatus & STATUS_CTRL)) {
			kp.hascmd = true;
		}
		if (cmd == KBDC_KPDEL) {
			kp.character = (u32int)'.';
		} else {
			kp.character = (u32int)'0' + (cmd - KBDC_KPINSERT);
		}
	} else if (cmd == KBDC_KPSLASH) {
		kp.haschar = true;
		if ((kbdstatus & STATUS_ALT) or (kbdstatus & STATUS_CTRL)) {
			kp.hascmd = true;
		}
		kp.character = (u32int)'/';
	} else if (cmd == KBDC_ALT) {
		kbdstatus |= STATUS_ALT;
	} else if (cmd == KBDC_ALTGR) {
		kbdstatus |= STATUS_ALTGR;
	} else if (cmd == KBDC_LEFTCTRL or cmd == KBDC_RIGHTCTRL) {
		kbdstatus |= STATUS_CTRL;
	} else if (cmd == KBDC_LEFTSHIFT or cmd == KBDC_RIGHTSHIFT) {
		kbdstatus |= STATUS_SHIFT;
	} else if (cmd == KBDC_CAPSLOCK) {
		kbdstatus ^= STATUS_CAPS;
		updateLeds();
	} else if (cmd == KBDC_NUMLOCK) {
		kbdstatus ^= STATUS_NUM;
		updateLeds();
	} else if (cmd == KBDC_SCRLLOCK) {
		kbdstatus ^= STATUS_SCRL;
		updateLeds();
	}
	if (!kp.haschar) {
		kp.hascmd = true;
		kp.command = cmd;
	}
	process(kp);
}

void keyRelease(u8int scancode) {
	keypress_t kp;
	kp.pressed = false;
	kp.modifiers = kbdstatus & 0x0F;
	u8int cmd = ctrlkeys[scancode];
	scancode &= 0x7F;
	if (cmd == 0) cmd = ctrlkeys[scancode];
	if (cmd == 0) {
		kp.haschar = true;
		kp.character = 0;
		if ((kbdstatus & STATUS_ALT) or (kbdstatus & STATUS_CTRL)) {
			kp.hascmd = true;
		}
		if (((kbdstatus & STATUS_SHIFT) != 0) xor ((kbdstatus & STATUS_CAPS) != 0)) {
			if (kbdstatus & STATUS_ALTGR) {
				if (keymapShiftAltgr != NULL) kp.character = keymapShiftAltgr[scancode];
			} else {
				if (keymapCaps != NULL and (kbdstatus & STATUS_CAPS))
					kp.character = keymapCaps[scancode];
				else if (keymapShift != NULL)
					kp.character = keymapShift[scancode];
			}
		} else {
			if (kbdstatus & STATUS_ALTGR) {
				if (keymapAltgr != NULL) kp.character = keymapAltgr[scancode];
			} else {
				if (keymapNormal != NULL) kp.character = keymapNormal[scancode];
			}
		}
	} else if (cmd >= KBDC_KPINSERT and cmd <= KBDC_KPDEL and (kbdstatus & STATUS_NUM)) {
		kp.haschar = true;
		if ((kbdstatus & STATUS_ALT) or (kbdstatus & STATUS_CTRL)) {
			kp.hascmd = true;
		}
		if (cmd == KBDC_KPDEL) {
			kp.character = (u32int)'.';
		} else {
			kp.character = (u32int)'0' + (cmd - KBDC_KPINSERT);
		}
	} else if (cmd == KBDC_ALT) {
		kbdstatus &= ~STATUS_ALT;
	} else if (cmd == KBDC_ALTGR) {
		kbdstatus &= ~STATUS_ALTGR;
	} else if (cmd == KBDC_LEFTCTRL or cmd == KBDC_RIGHTCTRL) {
		kbdstatus &= ~STATUS_CTRL;
	} else if (cmd == KBDC_LEFTSHIFT or cmd == KBDC_RIGHTSHIFT) {
		kbdstatus &= ~STATUS_SHIFT;
	}
	if (!kp.haschar && cmd) {
		kp.hascmd = true;
		kp.command = cmd;
	}
	process(kp);
}

}
