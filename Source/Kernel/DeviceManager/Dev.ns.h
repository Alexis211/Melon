#ifndef DEF_DEV_NS_H
#define DEF_DEV_NS_H

#include <Devices/Device.proto.h>
#include <Library/Vector.class.h>

namespace Dev {
	void handleIRQ(registers_t regs, int irq);
	
	void registerDevice(Device* dev);
	void unregisterDevice(Device* dev);

	bool requestIRQ(Device* dev, int irq);

	Vector<Device*> findDevice(String _class = "");
}

#endif

