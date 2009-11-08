#ifndef DEF_DISPLAY_PROTO_H
#define DEF_DISPLAY_PROTO_H

#include <common.h>
#include <Devices/Device.proto.h>
#include <WChar.class.h>

namespace Disp { struct mode_t; }

class Display : public Device {
	public:
	virtual ~Display() {}

	virtual void getModes(Vector<Disp::mode_t> &to) = 0;
	virtual bool setMode(Disp::mode_t& mode) = 0;
	virtual void clear() = 0;

	//Text functions
	virtual void putChar(u16int line, u16int col, WChar c, u8int color) = 0;	//Color : <bg 4bits><fg 4bits>
	virtual void moveCursor(u16int line, u16int col) = 0;

	//Graphic functions
};

#endif
