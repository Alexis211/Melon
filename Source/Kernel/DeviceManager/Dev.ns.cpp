#include "Dev.ns.h"

namespace Dev {

Vector<Device*> devices;
Device* irqHandler[16] = { NULL };

void handleIRQ(registers_t regs, int irq) {
	if (irqHandler[irq] != NULL)
		irqHandler[irq]->handleIRQ(regs, irq);
}

void registerDevice(Device* dev) {
	unregisterDevice(dev);	//Bad things could have happened
	devices.push(dev);
}

void unregisterDevice(Device* dev) {
	for (u32int i = 0; i < devices.size(); i++) {
		if (devices[i] == dev) {
			devices[i] = devices.back();
			devices.pop();
			return;
		}
	}
}

bool requestIRQ(Device* dev, int irq) {
	if (irqHandler[irq] == NULL) {
		irqHandler[irq] = dev;
		return true;
	} else {
		return false;
	}
}

Vector<Device*> findDevices(String _class) {
	if (_class.empty()) return devices;
	Vector<Device*> ret;
	for (u32int i = 0; i < devices.size(); i++) {
		String devclass = devices[i]->getClass();
		if (devclass == _class) {
			ret.push(devices[i]);
		} else if (devclass.size() > _class.size()) {
			if (devclass.substr(0, _class.size()) == _class) ret.push(devices[i]);
		}
	}
	return ret;
}

Device* findDevice(String _class, u32int idx) {
	for (u32int i = 0; i < devices.size(); i++) {
		String devclass = devices[i]->getClass();
		if (devclass == _class or (devclass.size() > _class.size() and devclass.substr(0, _class.size()) == _class)) {
			if (idx == 0) {
				return devices[i];
			} else {
				idx--;
			}
		}
	}
	return NULL;
}

}
