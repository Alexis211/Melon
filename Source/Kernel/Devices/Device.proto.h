#ifndef DEF_DEVICE_PROTO_H
#define DEF_DEVICE_PROTO_H

#include <String.class.h>

#include <SyscallManager/IDT.ns.h>

class Device {
	public:
	virtual ~Device() {};
	virtual String getClass() = 0;	
	virtual String getName() = 0;	

	virtual void handleIRQ(registers_t regs, int irq) {};
};

#endif
