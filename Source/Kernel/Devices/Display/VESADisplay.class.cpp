#include "VESADisplay.class.h"
#include <DeviceManager/Disp.ns.h>

#include <MemoryManager/PhysMem.ns.h>

using namespace Disp;

/****************************************
 * 					COLOR HANDLING FUNCTIONS
 * 					*********************************************/

inline u16int rgbTo15(u32int color) {
	return (
			(((color >> 16 & 0xFF) / 8) << 10) |
			(((color >> 8 & 0xFF) / 8) << 5) |
			((color & 0xFF) / 8));
}

inline u32int rgbFrom15(u16int color) {
	return (
			(((color >> 10 & 0x1F) * 8) << 16) |
			(((color >> 5 & 0x1F) * 8) << 8) |
			((color & 0x1F) * 8));
}

inline u16int rgbTo16(u32int color) {
	return (
			(((color >> 16 & 0xFF) / 8) << 11) |
			(((color >> 8 & 0xFF) / 4) << 5) |
			((color & 0xFF) / 8));
}

inline u32int rgbFrom16(u16int color) {
	return (
			(((color >> 11 & 0x1F) * 8) << 16) |
			(((color >> 5 & 0x3F) * 4) << 8) |
			((color & 0x1F) * 8));
}

/****************************************
 * 					DEVICE INFORMATION RELATED FUNCTIONS
 * 					*********************************************/

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
		if (mode.bpp != 24 and mode.bpp != 16 and mode.bpp != 15 and mode.bpp != 8) continue;
		mode_t m; m.device = this;
		m.textCols = mode.Xres / C_FONT_WIDTH; m.textRows = mode.Yres / C_FONT_HEIGHT;
		m.identifier = modes[i];
		m.graphWidth = mode.Xres; m.graphHeight = mode.Yres; m.graphDepth = mode.bpp;
		to.push(m);
	}
}

/****************************************
 * 					MODE SELECTION FUNCTIONS
 * 					*********************************************/

bool VESADisplay::setMode(mode_t &mode) {
	if (mode.device != this) return false;
	m_currMode = getModeInfo(mode.identifier);
	v86_regs_t regs;

	//Set mode
	regs.ax = 0x4F02;
	regs.bx = mode.identifier | 0x4000;
	V86::biosInt(0x10, regs);
	if ((regs.ax & 0xFF00) != 0) return false;

	if (m_currMode.bpp == 8) {
		//Set palette data
		for (int i = 0; i < 16; i++) {
			m_8bitPalette[i].pixels = 0;
			m_8bitPalette[i].color = rgbTo15(consoleColor[i]);
			setPalette(i, consoleColor[i]);
		}
		for (int i = 16; i < 256; i++) {
			m_8bitPalette[i].pixels = 0;
			m_8bitPalette[i].color = 0;
		}
	}

	m_fb = (u8int*)0xF0000000;
	for (u32int i = 0; i < (u32int)(m_currMode.Yres * m_currMode.pitch); i += 0x1000) {
		kernelPageDirectory->map(
				kernelPageDirectory->getPage((u32int)(m_fb + i), true),
			   	(m_currMode.physbase + i) / 0x1000, false, false);
	}
	m_pixWidth = (m_currMode.bpp + 1) / 8;
	clear();
	return true;
}

void VESADisplay::unsetMode() {
	for (u32int i = 0; i < (u32int)(m_currMode.Yres * m_currMode.pitch); i += 0x1000) {
		page_t* p = kernelPageDirectory->getPage((u32int)(m_fb + i), false);
		if (p != 0) p->present = 0, p->frame = 0;
	}
}

void VESADisplay::clear() {
	for (u32int* i = (u32int*)(memPos(0, 0)); i < (u32int*)(memPos(m_currMode.Xres, 0)); i++) {
		*i = 0;
	}
}
/****************************************
 * 					8BIT PALLET HANDLING
 * 					*********************************************/

void VESADisplay::setPalette(u8int id, u32int color) {
		Sys::outb(0x03C6, 0xFF);
		Sys::outb(0x03C8, id);
		Sys::outb(0x03C9, ((color >> 16) & 0xFF) / 4);
		Sys::outb(0x03C9, ((color >> 8) & 0xFF) / 4);
		Sys::outb(0x03C9, (color & 0xFF) / 4);
}

u8int VESADisplay::get8Bit(u32int color) {
	u16int c = rgbTo15(color);
	c &= ~0x0C63;	//Make the color very approximate (keep only 3bits per primary color)
	for (u16int i = 0; i < 256; i++) {
		if (m_8bitPalette[i].color == c) {
			return i;
		}
	}
	for (u16int i = 16; i < 256; i++) {
		if (m_8bitPalette[i].pixels == 0) {
			m_8bitPalette[i].color = c;
			setPalette(i, rgbFrom15(c));
			return i;
		}
	}
	return 0;
}

/****************************************
 * 					DRAWING FUNCTIONS
 * 					*********************************************/

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
		*p.w = rgbTo15(c); 
	} else if (m_currMode.bpp == 16) {
		*p.w = rgbTo16(c);
	} else if (m_currMode.bpp == 8) {
		m_8bitPalette[*p.c].pixels--;
		*p.c = get8Bit(c);
		m_8bitPalette[*p.c].pixels++;
	}
}

u32int VESADisplay::getPix(u16int x, u16int y) {
	if (x >= m_currMode.Xres or y >= m_currMode.Yres) return 0;
	u32int ret = 0;
	union {
		u8int* c;
	   	u16int* w;
		u32int* d;
	} p = {memPos(x, y)};
	if (m_currMode.bpp == 24) {
		ret = *p.d & 0x00FFFFFF;
	} else if (m_currMode.bpp == 15) {
		ret = rgbFrom15(*p.w);
	} else if (m_currMode.bpp == 16) {
		ret = rgbFrom16(*p.w);
	} else if (m_currMode.bpp == 8) {
		ret = rgbFrom15(m_8bitPalette[*p.c].color); 
	}
	return ret;
}

void VESADisplay::drawChar(u16int line, u16int col, WChar c, u8int color) {
	u8int ch = c.toAscii();
	if (ch == 0) return;
	u16int sx = col * C_FONT_WIDTH, sy = line * C_FONT_HEIGHT;
	u32int fgcolor = 1, bgcolor = 0;

	if (m_currMode.bpp == 24) {
		fgcolor = consoleColor[color & 0xF];
		bgcolor = consoleColor[(color >> 4) & 0xF];
	} else if (m_currMode.bpp == 15) {
		fgcolor = rgbTo15(consoleColor[color & 0xF]);
		bgcolor = rgbTo15(consoleColor[(color >> 4) & 0xF]);
	} else if (m_currMode.bpp == 16) {
		fgcolor = rgbTo16(consoleColor[color & 0xF]);
		bgcolor = rgbTo16(consoleColor[(color >> 4) & 0xF]);
	} else if (m_currMode.bpp == 8) {
		fgcolor = color & 0xF;
		bgcolor = (color >> 4) & 0xF;
	}

	if (c == WChar(" ")) {
		u8int* p = memPos(sx, sy);
		for (int y = 0; y < C_FONT_HEIGHT; y++) {
			if (m_pixWidth == 1) memset(p, bgcolor, 9);
			if (m_pixWidth == 2) memsetw((u16int*)p, bgcolor, 9);
			if (m_pixWidth == 3) {
				for (int i = 0; i < 9; i++) {
					p[0] = (bgcolor);
					p[1] = (bgcolor >> 8);
					p[2] = (bgcolor >> 16);
					p += 3;
				}
				p -= (9 * 3);
			}
			p += m_currMode.pitch;
		}
		return;
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
		} else if (m_pixWidth == 1) {
			m_8bitPalette[*pos.c].pixels--;
			*pos.c = bgcolor;
			for (int x = 0; x < 8; x++) {
				pos.c--;
				m_8bitPalette[*pos.c].pixels--;
				*pos.c = ((pixs & 1) != 0 ? fgcolor : bgcolor);
				pixs = pixs >> 1;
			}
		}
		y++;
	}
}

bool VESADisplay::textScroll(u16int line, u16int col, u16int height, u16int width, u8int color) {
	u8int* start = memPos(col * C_FONT_WIDTH, line * C_FONT_HEIGHT);
	u32int count = width * C_FONT_WIDTH * m_pixWidth;
	u32int diff = C_FONT_HEIGHT * m_currMode.pitch;
	putCsrBuff();
	for (int i = 0; i < (height - 1) * C_FONT_HEIGHT; i++) {
		memcpy(start, start + diff, count);
		start += m_currMode.pitch;
	}
	for (u32int i = 0; i < width; i++) {
		drawChar(line + height - 1, col + i, " ", color);
	}
	if (m_csrBuff.line == line + height - 1) m_csrBuff.line--;
	drawCsr();
	return true;
}
