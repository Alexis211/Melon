#include <Core/common.wtf.h>
#include <Core/multiboot.wtf.h>

#include <Devices/Display/VGATextOutput.class.h>
#include <DisplayManager/Disp.ns.h>

#include <Ressources/logo.cd>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

void kmain(multiboot_info_t* mbd, u32int magic) {
	DEBUG("Entering kmain.");

	VGATextOutput vgaout;

	Disp::setDisplay(&vgaout);

	for (int i = 0; i < melonLogoLines; i++) {
		int startCol = (Disp::textCols() / 2) - (melonLogoCols / 2);
		for (int j = 0; j < melonLogoCols; j++) {
			Disp::putChar(i + 2, j + startCol, melonLogo[i][j], 0x07);
		}
	}

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("Error with multiboot header.");
	}

	while(1);
}
