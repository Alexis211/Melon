#ifndef DEF_KBD_NS_H
#define DEF_KBD_NS_H

#include <common.h>
#include <WChar.class.h>
#include <Kbd.iface.h>

class VirtualTerminal;

namespace Kbd {
	void setFocus(VirtualTerminal* vt);
	bool loadKeymap(String lang);
	void updateLeds();
	void keyPress(u8int scancode);
	void keyRelease(u8int scancode);
}

#endif

