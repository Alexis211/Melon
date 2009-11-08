//This file contains the kernel's main procedure

#include <common.h>
#include <Core/multiboot.wtf.h>

#include <Devices/Display/VGATextOutput.class.h>
#include <Devices/Keyboard/PS2Keyboard.class.h>
#include <Devices/Floppy/FloppyDrive.class.h>
#include <Devices/Timer.class.h>
#include <DeviceManager/Disp.ns.h>
#include <DeviceManager/Dev.ns.h>
#include <DeviceManager/Kbd.ns.h>
#include <DeviceManager/Time.ns.h>
#include <VTManager/ScrollableVT.class.h>
#include <VTManager/FileVT.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <MemoryManager/GDT.ns.h>
#include <TaskManager/Task.ns.h>
#include <UserManager/Usr.ns.h>
#include <SyscallManager/IDT.ns.h>
#include <String.class.h>
#include <ByteArray.class.h>
#include <VFS/Part.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>
#include <VFS/FileNode.class.h>
#include <VFS/VFS.ns.h>
#include <VFS/DirectoryNode.class.h>
#include <Core/Log.ns.h>
#include <Shell/KernelShell.class.h>

#include <Ressources/Graphics/logo.text.cxd>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

#define INFO(vt) vt->setColor(KVT_FGCOLOR); *vt << " - "; vt->setColor(KVT_LIGHTCOLOR);
#define PROCESSING(vt, m) vt->setColor(KVT_BLECOLOR); *vt << " > "; vt->setColor(KVT_FGCOLOR); *vt << m; \
	vt->setCursorCol(60); vt->setColor(KVT_LIGHTCOLOR); *vt << ": ";
#define OK(vt) vt->setColor(KVT_FGCOLOR); *vt << "[ "; vt->setColor(KVT_OKCOLOR); *vt << "OK"; vt->setColor(KVT_FGCOLOR); *vt << " ]\n";

u32int logoAnimation(void* p) {
	SimpleVT& vt = *((SimpleVT*)p);
	vt.setColor(8);
	u8int *wat = new u8int[melonLogoLines * melonLogoCols];
	for (int x = 0; x < melonLogoCols; x++) {
		for (int y = 0; y < melonLogoLines; y++) {
			wat[x * melonLogoLines + y] = melonLogo[y][x];
			vt.putChar(y, x, " ");
		}
	}
	vt.setColor(TXTLOGO_FGCOLOR);
	u32int s = 32;
	for (int i = 0; i < 255; i++) {
		for (int x = 0; x < (i < melonLogoCols ? i : melonLogoCols); x++) {
			for (int y = 0; y < melonLogoLines; y++) {
				if (wat[x * melonLogoLines + y] != melonLogo[y][x]) {
					wat[x * melonLogoLines + y]++;
					if (wat[x * melonLogoLines + y] > melonLogo[y][x] && (y + x) % 2 == 1)
						wat[x * melonLogoLines + y] += 2;
					if (wat[x * melonLogoLines + y] >= 127)
						wat[x * melonLogoLines + y] = 32;
					vt.setColor(7);
					vt.putChar(y, x, wat[x * melonLogoLines + y]);
				} else {
					vt.setColor(TXTLOGO_FGCOLOR);
					vt.putChar(y, x, wat[x * melonLogoLines + y]);
				}
			}
		}
		vt.setColor(8);
		if (i < melonLogoCols) {
			for (int y = 0; y < melonLogoLines; y++) {
				s += 102;
				while (s > 127) s -= (127 - 33);
				wat[(i + (y%3)) * melonLogoLines + y] = s;
				vt.putChar(y, (i + (y%3)), wat[(i + (y%3)) * melonLogoLines + y]);
			}
		}
		Task::currThread()->sleep(20);
	}
	delete wat;
	return 0;
}

void kmain(multiboot_info_t* mbd, u32int magic) {
	DEBUG("Entering kmain.");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		Mem::placementAddress = (u32int)&end;	//Setup basic stuff so that PANIC will work
		VGATextOutput *vgaout = new VGATextOutput();
		Disp::setDisplay(vgaout);
		PANIC("Error with multiboot header.");
	}
	
	//Setup placement address so that we can use new without overwriting modules
	Mem::placementAddress = (u32int)&end;
	mbd->cmdline += 0xC0000000; mbd->mods_addr += 0xC0000000; //Take stuff into acount
	module_t *mods = (module_t*)mbd->mods_addr;
	for (u32int i = 0; i < mbd->mods_count; i++) {
		mods[i].mod_start += 0xC0000000;
		mods[i].mod_end += 0xC0000000;
		if (mods[i].mod_end > Mem::placementAddress) 
			Mem::placementAddress = mods[i].mod_end + 0x1000;
	}

	//Create text output
	VGATextOutput *vgaout = new VGATextOutput();
	Disp::setDisplay(vgaout);

	//Create a VT for handling the Melon bootup logo
	SimpleVT *melonLogoVT = new SimpleVT(melonLogoLines, melonLogoCols, TXTLOGO_FGCOLOR, TXTLOGO_BGCOLOR);
	melonLogoVT->map(1);

	//Create a VT for logging what kernel does
	SimpleVT *kvt = new ScrollableVT(3, 69, 10, KVT_FGCOLOR, KVT_BGCOLOR);
	kvt->map(15);

	INFO(kvt); *kvt << "Lower ram : " << (s32int)mbd->mem_lower << "k, upper : " << (s32int)mbd->mem_upper << "k.\n";
	INFO(kvt); *kvt << "Placement address : " << (u32int)Mem::placementAddress << "\n";

	PROCESSING(kvt, "Loading IDT...");
	IDT::init(); OK(kvt);

	PROCESSING(kvt, "Initializing paging...");
	u32int totalRam = ((mbd->mem_upper + mbd->mem_lower) * 1024);
	PhysMem::initPaging(totalRam); OK(kvt);

	INFO(kvt); *kvt << "Total ram : " << (s32int)(totalRam / 1024) << "k (" << (s32int)(totalRam / (1024 * 1024)) << "M).\n";
	PROCESSING(kvt, "Initializing real GDT and cleaning page directory...");
	GDT::init();
	PhysMem::removeTemporaryPages(); OK(kvt);

	PROCESSING(kvt, "Creating kernel heap...");
	Mem::createHeap(); OK(kvt);
	INFO(kvt); *kvt << "Free frames : " << (s32int)PhysMem::free() << "/" << (s32int)PhysMem::total() << "\n";

	PROCESSING(kvt,"Initializing PIT...");
	Dev::registerDevice(new Timer()); OK(kvt);

	PROCESSING(kvt, "Initializing multitasking...");
	Task::initialize(String((char*)mbd->cmdline), kvt); 
	new Thread(logoAnimation, (void*)melonLogoVT, true); OK(kvt);

	PROCESSING(kvt, "Mounting first module as ramfs on root directory...");
	FileSystem* fs = RamFS::mount((u8int*)mods[0].mod_start, 1024 * 1024, NULL);
	DirectoryNode* cwd;
	cwd = fs->getRootNode();
	Task::currProcess()->setCwd(cwd);
	VFS::setRootNode(cwd); OK(kvt);

	PROCESSING(kvt, "Setting up logs...");
	Log::init(KL_STATUS); OK(kvt);
	INFO(kvt); *kvt << "Logs are now going to files in /System/Logs/\n";

	Dev::registerDevice(vgaout);
	Log::log(KL_STATUS, "kmain : Registered textual VGA output");

	Dev::registerDevice(new PS2Keyboard());	//Initialize keyboard driver
	if (!Kbd::loadKeymap("fr")) Log::log(KL_ERROR, "kmain : could not load french keymap.");
	Kbd::setFocus(kvt);	//Set focus to virtual terminal
	Log::log(KL_STATUS, "kmain : Keyboard set up");

	FloppyController::detect();
	Log::log(KL_STATUS, "kmain : Floppy drives detected");

	Usr::load();
	Log::log(KL_STATUS, "kmain : User list loaded");

	asm volatile("sti");
	Log::log(KL_STATUS, "kmain : Interrupts enabled.");

	/*
	new KernelShell(cwd);	//No need to save that in a var, it is automatically destroyed anyways
	Log::log(KL_STATUS, "kmain : Kernel shell launched");
	//kvt->unmap();

	while (KernelShell::getInstances() > 0) {
		Task::currThread()->sleep(100);
	}

	Log::log(KL_STATUS, "kmain : All kernel shells finished. Halting.");
	Sys::halt();
	*/

	Process* p = Process::run("/System/Applications/PaperWork.app", 0);
	if (p == 0) {
		PANIC("Could not launch PaperWork !");
	} else {
		VirtualTerminal* vt = new ScrollableVT(15, 76, 200, SHELL_FGCOLOR, SHELL_BGCOLOR);
		Kbd::setFocus(vt);
		((ScrollableVT*)vt)->map(9);
		p->setInVT(vt);
		p->setOutVT(vt);
		p->start();
		while (p->getState() != P_FINISHED) Task::currThread()->sleep(100);
		PANIC("PaperWork finished.");
	}

	PANIC("END OF KMAIN");
}
