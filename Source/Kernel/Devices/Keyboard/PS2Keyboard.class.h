#ifndef DEF_PS2KEYBOARD_CLASS_H
#define DEF_PS2KEYBOARD_CLASS_H

#include <Devices/Keyboard/Keyboard.proto.h>

class PS2Keyboard : public Keyboard {
	private:
	bool m_escaped;
	
	public:
	PS2Keyboard();
	String getClass();
	String getName();	
	void handleIRQ(registers_t regs, int irq);
	void updateLeds(u32int kbdstatus);
};

#endif
