#ifndef DEF_KBD_NS_H
#define DEF_KBD_NS_H

#include <common.h>
#include <WChar.class.h>

//Used by variable kbdstatus
#define STATUS_SCRL 0x40
#define STATUS_NUM  0x20
#define STATUS_CAPS 0x10
#define STATUS_SHIFT 0x08
#define STATUS_CTRL 0x04
#define STATUS_ALT  0x02
#define STATUS_ALTGR 0x01

//Used in control keys keymap. The ones > 100 are modifiers and are not supposed to be sent to applications.
#define KBDC_LEFTCTRL 101
#define KBDC_RIGHTCTRL 102
#define KBDC_ALT 103
#define KBDC_ALTGR 104
#define KBDC_LEFTSUP 5	//Super = windows
#define KBDC_RIGHTSUP 6
#define KBDC_MENU 7
#define KBDC_LEFTSHIFT 108
#define KBDC_RIGHTSHIFT 109
#define KBDC_CAPSLOCK 110
#define KBDC_TAB 11
#define KBDC_ENTER 12
#define KBDC_BACKSPACE 13

#define KBDC_KPINSERT 14		//Key 0/insert
#define KBDC_KPEND 15			//Key 1/end
#define KBDC_KPDOWN 16			//Key 2/down
#define KBDC_KPPGDOWN 17		//Key 3/pgdown
#define KBDC_KPLEFT 18			//Key 4/left
#define KBDC_KP5 19				//Key 5			this is sent to receiving application, but must be ignored
#define KBDC_KPRIGHT 20			//Key 6/right
#define KBDC_KPHOME 21			//Key 7/home
#define KBDC_KPUP 22			//Key 8/up
#define KBDC_KPPGUP 23			//Key 9/pgup
#define KBDC_KPDEL 24			//Key ./del

#define KBDC_HOME 25
#define KBDC_END 26
#define KBDC_INSERT 27
#define KBDC_DEL 28
#define KBDC_PGUP 29
#define KBDC_PGDOWN 30
#define KBDC_UP 31
#define KBDC_DOWN 32
#define KBDC_LEFT 33
#define KBDC_RIGHT 34

#define KBDC_NUMLOCK 135
#define KBDC_SCRLLOCK 136
#define KBDC_PRTSCN 37		//Print screen
#define KBDC_SYSREQ 38

#define KBDC_ESCAPE 40
#define KBDC_F1 41
#define KBDC_F2 42
#define KBDC_F3 43
#define KBDC_F4 44
#define KBDC_F5 45
#define KBDC_F6 46
#define KBDC_F7 47
#define KBDC_F8 48
#define KBDC_F9 49
#define KBDC_F10 50
#define KBDC_F11 51
#define KBDC_F12 52

//This is a special case. Keycode is escaped, Keyboard:: will send a 0xB5 keycode, that must not be mixed up with 0x35
#define KBDC_KPSLASH 53

class VirtualTerminal;

namespace Kbd {
	//== Possible cases for keypress_t :
	// - hascmd && !haschar : this is a command key press/release (all grey keys except alt/ctrl/altgr/shift)
	// - haschar && !hascmd : this is a character key press/release. Modifiers can haz STATUS_SHIFT or STATUS_ALTGR
	// - haschar && hascmd : this is a character key press, but with ctrl and/or alt. See that in modifiers.
	// - !haschar && !hascmd : invalid keypress
	struct keypress_t {
		bool pressed;
		bool hascmd;
		bool haschar;
		u8int modifiers;
		u8int command;	
		WChar character; //is 0 if !haschar
		keypress_t() : hascmd(false), haschar(false), command(0), character('\0') {};
	};

	void setFocus(VirtualTerminal* vt);
	bool loadKeymap(String lang);
	void updateLeds();
	void keyPress(u8int scancode);
	void keyRelease(u8int scancode);
}

#endif

