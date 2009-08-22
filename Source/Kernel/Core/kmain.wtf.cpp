//This file contains the kernel's main procedure

#include <Core/common.wtf.h>
#include <Core/multiboot.wtf.h>

#include <Devices/Display/VGATextOutput.class.h>
#include <Devices/Timer.class.h>
#include <DeviceManager/Disp.ns.h>
#include <DeviceManager/Dev.ns.h>
#include <VTManager/VirtualTerminal.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <MemoryManager/GDT.ns.h>
#include <SyscallManager/IDT.ns.h>
#include <Library/String.class.h>

#include <Ressources/logo.cd>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

void kmain(multiboot_info_t* mbd, u32int magic) {
	DEBUG("Entering kmain.");

	Mem::placementAddress = (u32int)&end;
	mbd->cmdline += 0xC0000000; mbd->mods_addr += 0xC0000000; //Take stuff into acount
	module_t *mods = (module_t*)mbd->mods_addr;
	for (u32int i = 0; i < mbd->mods_count; i++) {
		mods[i].mod_start += 0xC0000000;
		mods[i].mod_end += 0xC0000000;
		if (mods[i].mod_end > Mem::placementAddress) 
			Mem::placementAddress = mods[i].mod_end + 0x1000;
	}

	VGATextOutput *vgaout = new VGATextOutput();

	Disp::setDisplay(vgaout);

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("Error with multiboot header.");
	}
	
	VirtualTerminal *melonLogoVT = new VirtualTerminal(melonLogoLines, melonLogoCols, 7, 0);
	for (int i = 0; i < melonLogoLines; i++) {
		for (int j = 0; j < melonLogoCols; j++) {
			melonLogoVT->putChar(i, j, melonLogo[i][j]);
		}
	}
	melonLogoVT->map(2);

	VirtualTerminal *kvt = new VirtualTerminal(12, 40, 0, 2);
	kvt->map(melonLogoLines + 4);

	*kvt << "* Kernel initializing in HIGHER HALF!\n";

	*kvt << "- Lower ram : " << (s32int)mbd->mem_lower << "k, upper : " << (s32int)mbd->mem_upper << "k.\n";
	*kvt << "- Kernel command line : " << (u32int)mbd->cmdline << "\n";
	*kvt << "- Modules@" << (u32int)mbd->mods_addr << ", mbd@" << (u32int)mbd << "\n";
	*kvt << "- Placement address : " << (u32int)Mem::placementAddress << "\n";

	*kvt << "> Loading IDT...";
	IDT::init();

	*kvt << "OK.\n> Initializing paging...";
	u32int totalRam = ((mbd->mem_upper + 1024) * 1024);
	PhysMem::initPaging(totalRam);

	*kvt << "OK.\n- Total ram : " << (s32int)(totalRam / 1024) << "k.\n";
	GDT::init();
	*kvt << "> GDT OK. Cleaning page directory...";
	PhysMem::removeTemporaryPages();

	*kvt << "OK.\n> Creating heap...";
	Mem::createHeap();
	*kvt << "OK.\n";
	*kvt << "- Free frames : " << (s32int)PhysMem::free() << "/" << 
		(s32int)PhysMem::total();
	
	*kvt << "\n> Registering vgaout...";
	Dev::registerDevice(vgaout);

	*kvt << "OK.\n> Initializing PIT...";
	Dev::registerDevice(new Timer());

	*kvt << "OK.\n";

	asm volatile("sti");

	//PANIC("END OF KMAIN");
}
