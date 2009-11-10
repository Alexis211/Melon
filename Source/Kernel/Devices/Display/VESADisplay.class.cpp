#include "VESADisplay.class.h"
#include <DeviceManager/Disp.ns.h>

extern v86_function_t vesa_int;	//in vga-vesa.wtf.asm

using namespace Disp;

String VESADisplay::getClass() {
	return "display.vesa";
}

String VESADisplay::getName() {
	return "Standard VESA display";
}

vbe_controller_info_t VESADisplay::getCtrlrInfo() {
	vbe_controller_info_t *info = (vbe_controller_info_t*)V86::alloc(sizeof(vbe_controller_info_t));
	info->signature[0] = 'V'; info->signature[1] = 'B'; info->signature[2] = 'E'; info->signature[3] = '2';
	info->videomodes = 0;
	registers_t regs;
	regs.eax = 0x00004F00;
	regs.esi = LIN_SEG(info);
	regs.edi = LIN_OFF(info);
	V86::run(vesa_int, regs, 0);
	if (regs.eax != 0x004F) PANIC("Something went wrong in detecting VBE modes.");
	if (info->signature[3] != 'A') PANIC("No vesa sinature");
	return *info;
}

vbe_mode_info_t VESADisplay::getModeInfo(u16int id) {
	vbe_mode_info_t *mode = (vbe_mode_info_t*)V86::alloc(sizeof(vbe_mode_info_t));
	CMem::memset((u8int*)mode, 0, sizeof(vbe_mode_info_t));
	registers_t regs;
	regs.eax = 0x00004F01;
	regs.ecx = id;
	regs.esi = LIN_SEG(mode);
	regs.edi = LIN_OFF(mode);
	V86::run(vesa_int, regs, 0);
	return *mode;
}

void VESADisplay::getModes(Vector<mode_t> &to) {
	vbe_controller_info_t info = getCtrlrInfo();

	u16int *modes = (u16int*)(((info.videomodes & 0xFFFF0000) >> 12) | ((info.videomodes) & 0x0000FFFF));
	for (int i = 0; i < 64; i++) {
		if (modes[i] == 0xFFFF) break;
		vbe_mode_info_t mode = getModeInfo(modes[i]);

		if ((mode.attributes & 0x19) != 0x19) continue;
		if (mode.planes != 1) continue;
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
	registers_t regs;
	regs.eax = 0x00004F02;
	regs.ebx = mode.identifier;
	V86::run(vesa_int, regs, 0);
	return true;
}

u8int* VESADisplay::memPos(u16int x, u16int y) {
	u32int addr = y * m_currMode.pitch + x * (m_currMode.bpp / 8);

	u8int *base = (u8int*)(((m_currMode.physbase & 0xFFFF0000) >> 12) | (m_currMode.physbase & 0x0000FFFF));

	return base + addr;
}

void VESADisplay::clear() {
		for (u16int y = 0; y < m_currMode.Yres; y++) {
	for (u16int x = 0; x < m_currMode.Xres; x++) {
			putPix(x, y, 0x77777777);
		}
	}
}

void VESADisplay::putPix(u16int x, u16int y, u32int c) {
	u32int addr = y * m_currMode.pitch + x * (m_currMode.bpp / 8);
	int banksize = m_currMode.granularity*1024;
	int banknumber = addr / banksize;
	int bankoffset = addr % banksize;

	if (banknumber != b) {	
		registers_t r;
		r.eax = 0x4F05;
		r.ebx = 0;
		r.edx = banknumber;
		V86::run(vesa_int, r, 0);
		b = banknumber;
	}

	u8int* a = (u8int*)(0xA0000 + bankoffset);
	a[2] = (c >> 16) & 0xFF;
	a[1] = (c >> 8) & 0xFF;
	a[0] = c & 0xFF;
}

u32int VESADisplay::getPix(u16int x, u16int y) {
	u32int addr = y * m_currMode.pitch + x * (m_currMode.bpp / 8);
	int banksize = m_currMode.granularity*1024;
	int banknumber = addr / banksize;
	int bankoffset = addr % banksize;

	if (banknumber != b) {	
		registers_t r;
		r.eax = 0x4F05;
		r.ebx = 0;
		r.edx = banknumber;
		V86::run(vesa_int, r, 0);
		b = banknumber;
	}

	u32int ret;

	u8int* a = (u8int*)(0xA0000 + bankoffset);
	ret = (a[2] << 16) | (a[1] << 8) | a[0];
	return ret;
}
