#include "PS2Keyboard.class.h"
#include <DeviceManager/Dev.ns.h>
#include <DeviceManager/Kbd.ns.h>

using namespace Sys;

PS2Keyboard::PS2Keyboard() {
	Dev::requestIRQ(this, 1);
	m_escaped = false;
}

String PS2Keyboard::getClass() {
	return "keyboard.ps2";
}

String PS2Keyboard::getName() {
	return "Standard PS2 keyboard";
}

void PS2Keyboard::handleIRQ(registers_t regs, int irq) {
	if (irq == 1) {
		u8int scancode = inb(0x60);
		if (scancode == 0xE0) {
			m_escaped = true;
		} else {
			if (scancode & 0x80) {
				if (m_escaped) {
					Kbd::keyRelease(scancode);
				} else {
					Kbd::keyRelease(scancode & 0x7F);
				}
			} else {
				if (m_escaped) {
					Kbd::keyPress(scancode | 0x80);
				} else {
					Kbd::keyPress(scancode);
				}
			}
		}
	}
}

void PS2Keyboard::updateLeds(u32int kbdstatus) {
	u8int temp = 0;
	if (kbdstatus & STATUS_SCRL)
		temp |= 1;
	if (kbdstatus & STATUS_NUM)
		temp |= 2;
	if (kbdstatus & STATUS_CAPS)
		temp |= 4;
	outb(0x60, temp);
}
