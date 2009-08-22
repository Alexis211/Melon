#include "Timer.class.h"

#include <DeviceManager/Dev.ns.h>
#include <DeviceManager/Time.ns.h>

using namespace Sys; //For outb

Timer::Timer(u8int frequency) {
	m_ticks = 0;
	m_seconds = 0;

	Dev::requestIRQ(this, 0);
	Time::setTimer(this);
	setFrequency(frequency);
}

String Timer::getClass() {
	return String("timer");
}

String Timer::getName() {
	return String("Programmable interrupt timer");
}

void Timer::setFrequency(u8int frequency) {
	m_frequency = frequency;
	u32int divisor = 1193180 / (u32int)frequency;
	outb(0x43, 0x36);
	
	u8int l = (u8int)(divisor & 0xFF);
	u8int h = (u8int)( (divisor >> 8) & 0xFF);

	outb(0x40, l);
	outb(0x40, h);
}

u32int Timer::uptime() {
	return m_seconds;
}

u32int Timer::time() {
	return (m_seconds * 1000) + ((m_ticks * 1000) / m_frequency);
}

void Timer::handleIRQ(registers_t registers, int irq) {
	if (irq == 0) {
		m_ticks++;
		if (m_ticks == m_frequency) {
			m_ticks = 0;
			m_seconds++;
		}
		//Switching task is called in Dev::handleInterrupt
	}
}


