#ifndef DEF_KEYBOARD_PROTO_H
#define DEF_KEYBOARD_PROTO_H

#include <Devices/Device.proto.h>

class Keyboard : public Device {
	public:
	virtual ~Keyboard() {}
	virtual void updateLeds(u32int kbdstatus) = 0;
};

#endif
