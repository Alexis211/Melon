#ifndef DEF_DISPLAY_PROTO_H
#define DEF_DISPLAY_PROTO_H

#include <common.h>
#include <Devices/Device.proto.h>
#include <WChar.class.h>

class Display : public Device {
	public:
	virtual ~Display() {}
	virtual u16int textCols() = 0;
	virtual u16int textRows() = 0;
	virtual void putChar(u16int line, u16int col, WChar c, u8int color) = 0;	//Color : <bg 4byte><fg 4byte>
	virtual void moveCursor(u16int line, u16int col) = 0;
	virtual void clear() = 0;
};

#endif
