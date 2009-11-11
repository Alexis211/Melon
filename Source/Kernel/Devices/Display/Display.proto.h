#ifndef DEF_DISPLAY_PROTO_H
#define DEF_DISPLAY_PROTO_H

#include <common.h>
#include <Devices/Device.proto.h>
#include <WChar.class.h>

namespace Disp { struct mode_t; }

class VirtualTerminal;

class Display : public Device {
	public:
	virtual ~Display() {}

	virtual void getModes(Vector<Disp::mode_t> &to) = 0;
	virtual bool setMode(Disp::mode_t& mode) = 0;
	virtual void clear() = 0;

	//Text functions
	virtual void putChar(u16int line, u16int col, WChar c, u8int color) = 0;	//Color : <bg 4bits><fg 4bits>
	virtual void moveCursor(u16int line, u16int col) = 0;

	//Graphic functions, can stay unimplemented for textual displays
	virtual void putPix(u16int x, u16int y, u32int color) {}
	virtual u32int getPix(u16int x, u16int y) { return 0; }
	//Advanced graphic functions. These have a simple implementation in GraphicDisplay,
	//and should have an optimized version in each individual driver. This is also the 
	//case of the graphical putChar and moveCursor.
	//drawLine, drawCircle, ...
};

#endif
