#include "VESADisplay.class.h"
#include <DeviceManager/Disp.ns.h>

#include <MemoryManager/PhysMem.ns.h>

using namespace Disp;

String VESADisplay::getClass() {
	return "display.vesa";
}

String VESADisplay::getName() {
	return "Standard VESA display";
}

vbe_controller_info_t VESADisplay::getCtrlrInfo() {
	V86::map();
	vbe_controller_info_t *info = (vbe_controller_info_t*)V86::alloc(sizeof(vbe_controller_info_t));
	info->signature[0] = 'V'; info->signature[1] = 'B'; info->signature[2] = 'E'; info->signature[3] = '2';
	info->videomodes = 0;
	v86_regs_t regs;
	regs.ax = 0x4F00;
	regs.es = LIN_SEG(info);
	regs.di = LIN_OFF(info);
	V86::biosInt(0x10, regs);
	if (regs.ax != 0x004F) PANIC("Something went wrong in detecting VBE modes.");
	if (info->signature[3] != 'A') PANIC("No vesa sinature");
	return *info;
}

vbe_mode_info_t VESADisplay::getModeInfo(u16int id) {
	V86::map();
	vbe_mode_info_t *mode = (vbe_mode_info_t*)V86::alloc(sizeof(vbe_mode_info_t));
	CMem::memset((u8int*)mode, 0, sizeof(vbe_mode_info_t));
	v86_regs_t regs;
	regs.ax = 0x00004F01;
	regs.cx = id;
	regs.es = LIN_SEG(mode);
	regs.di = LIN_OFF(mode);
	V86::biosInt(0x10, regs);
	return *mode;
}

void VESADisplay::getModes(Vector<mode_t> &to) {
	vbe_controller_info_t info = getCtrlrInfo();

	u16int *modes = (u16int*)(((info.videomodes & 0xFFFF0000) >> 12) | ((info.videomodes) & 0x0000FFFF));
	for (int i = 0; i < 64; i++) {
		if (modes[i] == 0xFFFF) break;
		vbe_mode_info_t mode = getModeInfo(modes[i]);

		if ((mode.attributes & 0x90) != 0x90) continue;
		if (mode.memory_model != 4 and mode.memory_model != 6) continue;
		//if (mode.bpp != 24) continue;
		mode_t m; m.device = this;
		m.textCols = mode.Xres / C_FONT_WIDTH; m.textRows = mode.Yres / C_FONT_HEIGHT;
		m.identifier = modes[i];
		m.graphWidth = mode.Xres; m.graphHeight = mode.Yres; m.graphDepth = mode.bpp;
		to.push(m);
	}
}

bool VESADisplay::setMode(mode_t &mode) {
	if (mode.device != this) return false;
	m_currMode = getModeInfo(mode.identifier);
	v86_regs_t regs;
	regs.ax = 0x00004F02;
	regs.bx = mode.identifier | 0x4000;
	V86::biosInt(0x10, regs);

	m_fb = (u8int*)0xF0000000;
	for (u32int i = 0; i < (u32int)(m_currMode.Yres * m_currMode.pitch); i += 0x1000) {
		kernelPageDirectory->map(
				kernelPageDirectory->getPage((u32int)(m_fb + i), true),
			   	(m_currMode.physbase + i) / 0x1000, false, false);
	}
	m_pixWidth = (m_currMode.bpp + 1) / 8;
	return true;
}

void VESADisplay::clear() {
	for (u32int* i = (u32int*)(memPos(0, 0)); i < (u32int*)(memPos(m_currMode.Xres, 0)); i++) {
		*i = 0x77777777;
	}
}

void VESADisplay::putPix(u16int x, u16int y, u32int c) {
	if (x >= m_currMode.Xres or y >= m_currMode.Yres) return;
	union {
		u8int* c;
	   	u16int* w;
		u32int* d;
	} p = {memPos(x, y)};
	if (m_currMode.bpp == 24) {
		*p.d = (*p.d & 0xFF000000) | c;
	} else if (m_currMode.bpp == 15) {
		u32int r = (c & 0x00FF0000 >> 16) * 32 / 256,
			   g = (c & 0x0000FF00 >> 8) * 32 / 256,
			   b = (c & 0x000000FF) * 32 / 256;
		*p.w = (r << 10) | (g << 5) | b; 
	}
}

u32int VESADisplay::getPix(u16int x, u16int y) {
	if (x >= m_currMode.Xres or y >= m_currMode.Yres) return 0;
	u32int ret;
	union {
		u8int* c;
	   	u16int* w;
		u32int* d;
	} p = {memPos(x, y)};
	if (m_currMode.bpp == 24) {
		ret = *p.d & 0x00FFFFFF;
	} else if (m_currMode.bpp == 15) {
		u32int r = ((*p.w >> 10) & 0x1F) * 256 / 32,
			   g = ((*p.w >> 5) & 0x1F) * 256 / 32,
			   b = (*p.w & 0x1F) * 256 / 32;
		return (r << 16) | (g << 8) | b;
	}
	return ret;
}

//Advanced functions
void VESADisplay::drawChar(u16int line, u16int col, WChar c, u8int color) {
	u8int ch = c.toAscii();
	if (ch == 0) return;
	u16int sx = col * C_FONT_WIDTH, sy = line * C_FONT_HEIGHT;
	u32int fgcolor = consoleColor[color & 0xF], bgcolor = consoleColor[(color >> 4) & 0xF];

	if (m_pixWidth == 2) {
		u32int r = (fgcolor & 0x00FF0000 >> 16) * 32 / 256,
			   g = (fgcolor & 0x0000FF00 >> 8) * 32 / 256,
			   b = (fgcolor & 0x000000FF) * 32 / 256;
		fgcolor = (r << 10) | (g << 5) | b;
		r = (bgcolor & 0x00FF0000 >> 16) * 32 / 256,
		g = (bgcolor & 0x0000FF00 >> 8) * 32 / 256,
		b = (bgcolor & 0x000000FF) * 32 / 256;
		bgcolor = (r << 10) | (g << 5) | b;
	}
		

	int y = 0;
	for (u8int* p = memPos(sx, sy); p < memPos(sx, sy + C_FONT_HEIGHT); p += m_currMode.pitch) {
		union {
			u8int* c;
			u16int* w;
			u32int* d;
		} pos = {p + (8 * m_pixWidth)};
		u8int pixs = consoleFont[ch][y];
		if (m_pixWidth == 3) {
			*pos.d = (*pos.d & 0xFF000000) | bgcolor;
			for (int x = 0; x < 8; x++) {
				pos.c -= m_pixWidth;
				*pos.d = (*pos.d & 0xFF000000) | ((pixs & 1) != 0 ? fgcolor : bgcolor);
				pixs = pixs >> 1;
			}
		} else if (m_pixWidth == 2) {
			*pos.w = bgcolor;
			for (int x = 0; x < 8; x++) {
				pos.c -= m_pixWidth;
				*pos.w = ((pixs & 1) != 0 ? fgcolor : bgcolor);
				pixs = pixs >> 1;
			}
		}
		y++;
	}
}
