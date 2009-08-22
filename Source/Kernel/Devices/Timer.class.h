#ifndef DEF_TIMER_CLASS
#define DEF_TIMER_CLASS

#include <Devices/Device.proto.h>

class Timer : public Device {
	private:
	u8int m_frequency;
	u8int m_ticks;
	u32int m_seconds;

	public:
	Timer(u8int frequency = 100);

	void setFrequency(u8int frequency);

	String getClass();	
	String getName();

	u32int uptime();//Returns seconds since init
	u32int time();	//Returns miliseconds since init

	void handleIRQ(registers_t registers, int irq);
};

#endif
