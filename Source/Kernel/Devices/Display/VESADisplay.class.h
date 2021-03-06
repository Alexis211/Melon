#ifndef DEF_VESADISPLAY_CLASS_H
#define DEF_VESADISPLAY_CLASS_H

#include <Devices/Display/GraphicDisplay.proto.h>
#include <TaskManager/V86/V86.ns.h>

struct vbe_controller_info_t {
	char signature[4];             // == "VESA"
	s16int version;                 // == 0x0300 for VBE 3.0
	//s16int oemString[2];            // isa vbeFarPtr
	FARPTR oemString;
	u8int capabilities[4];
	FARPTR videomodes;
	s16int totalMemory;             // as # of 64KB blocks
} __attribute__((packed));

struct vbe_mode_info_t {
	u16int attributes;
	u8int winA, winB;
	u16int granularity;
	u16int winsize;
	u16int segmentA, segmentB;
	FARPTR realFctPtr;
	u16int pitch; // bytes per scanline

	u16int Xres, Yres;
	u8int Wchar, Ychar, planes, bpp, banks;
	u8int memory_model, bank_size, image_pages;
	u8int reserved0;

	u8int red_mask, red_position;
	u8int green_mask, green_position;
	u8int blue_mask, blue_position;
	u8int rsv_mask, rsv_position;
	u8int directcolor_attributes;

	u32int physbase;  // your LFB address ;)
	u32int reserved1;
	u16int reserved2;
} __attribute__ ((packed));

class VESADisplay : public GraphicDisplay {
	private:
	vbe_controller_info_t getCtrlrInfo();
	vbe_mode_info_t getModeInfo(u16int mode);

	vbe_mode_info_t m_currMode;

	int b, m_pixWidth;

	struct {
		u32int pixels;
		u16int color;
	} m_8bitPalette[256];

	u8int *m_fb;

	u8int* memPos(u16int x, u16int y) {
		u32int addr = y * m_currMode.pitch + x * m_pixWidth;
		return ((u8int*)m_fb) + addr;
	}

	u8int get8Bit(u32int color);
	void setPalette(u8int id, u32int color);

	public:
	String getClass();
	String getName();

	void getModes(Vector<Disp::mode_t> &to);
	bool setMode(Disp::mode_t& mode);
	void unsetMode();

	void clear();
	void putPix(u16int x, u16int y, u32int color);
	u32int getPix(u16int x, u16int y);

	//Advanced graphical functions, recoded for being optimized
	virtual void drawChar(u16int line, u16int col, WChar c, u8int color);
	bool textScroll(u16int line, u16int col, u16int height, u16int width, u8int color);
};

#endif
