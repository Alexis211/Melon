//This file contains the kernel's main procedure

#include <common.h>
#include <Core/multiboot.wtf.h>

#include <Devices/Display/VGATextOutput.class.h>
#include <Devices/Display/VESADisplay.class.h>
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
#include <Rand.ns.h>
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

void selectVideoMode(SimpleVT& v) {
	Disp::getModes();
	v << "\n\nPlease select a graphic mode in the list below:\n";

	for (u32int i = 0; i < Disp::modes.size(); i++) {
		Disp::mode_t& m = Disp::modes[i];
		v << (s32int)i << ":\t" << "Text " << m.textRows << "x" << m.textCols << "\t";
		if (m.graphWidth != 0 and m.graphHeight != 0) {
			v << "Graphics " << m.graphWidth << "x" << m.graphHeight << "x" << m.graphDepth << "\t";
		} else {
			v << "No graphics";
		}
		v.setCursorCol(50);
		v << m.device->getName() << "\n";
	}

	while (1) {
		v << "\nYour selection: ";
		String answer = v.readLine();
		u32int n = answer.toInt();
		v.unmap();
		if (n >= 0 and n < Disp::modes.size() and Disp::setMode(Disp::modes[n])) {
			return;
		} else {
			Disp::setMode(Disp::modes[1]);
			v.map();
			v << "Error while switching video mode, please select another one.";
		}
	}
}

void kmain(multiboot_info_t* mbd, u32int magic) {
	DEBUG("Entering kmain.");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		Mem::placementAddress = (u32int)&end;	//Setup basic stuff so that PANIC will work
		VGATextOutput *vgaout = new VGATextOutput();
		Disp::setText(vgaout);
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

	//**************************************	BASIC KERNEL SETUP

	//Create text output
	VGATextOutput *vgaout = new VGATextOutput();
	Disp::setText(vgaout);

	//Create a VT for logging what kernel does
	SimpleVT *kvt = new ScrollableVT(25, 80, 20, KVT_FGCOLOR, KVT_BGCOLOR);
	kvt->map(0, 0);
	*kvt << "Melon is loading...";

	IDT::init();		//Setup interrupts 

	u32int totalRam = ((mbd->mem_upper + mbd->mem_lower) * 1024);
	PhysMem::initPaging(totalRam);		//Setup paging 

	GDT::init();		//Initialize real GDT, not fake one from loader.wtf.asm
	PhysMem::removeTemporaryPages(); 	//Remove useless page mapping

	Mem::createHeap();		//Create kernel heap 
	Dev::registerDevice(vgaout);

	Dev::registerDevice(new Timer()); 	//Initialize timer
	String kcmdline((char*)mbd->cmdline);
	Task::initialize(kcmdline, kvt);	//Initialize multitasking 

	asm volatile("sti");

	//***************************************	PARSE COMMAND LINE
	
	Vector<String> opts = kcmdline.split(" ");
	String keymap = "fr", init = "/System/Applications/PaperWork.app";
	bool enableVESA = true;
	for (u32int i = 0; i < opts.size(); i++) {
		Vector<String> opt = opts[i].split(":");
		if (opt[0] == "vesa" && opt[1] != "enabled") enableVESA = false;
		if (opt[0] == "keymap") keymap = opt[1];
		if (opt[0] == "init") init = opt[1];
	}

	//*************************************** 	DEVICE SETUP

	if (enableVESA) Dev::registerDevice(new VESADisplay());
	FloppyController::detect();
	Dev::registerDevice(new PS2Keyboard());	//Initialize keyboard driver
	Kbd::setFocus(kvt);	//Set focus to virtual terminal

	//***************************************	MOUNT ROOT FILESYSTEM

	FileSystem* fs = RamFS::mount((u8int*)mods[0].mod_start, 1024 * 1024, NULL);
	DirectoryNode* cwd;
	cwd = fs->getRootNode();
	Task::currProcess()->setCwd(cwd);
	VFS::setRootNode(cwd);

	if (keymap != "builtin") {
		if (!Kbd::loadKeymap(keymap)) *kvt << "\nWARNING : Could not load keymap " << keymap << ", using built-in keymap instead.";
	}

	Log::init(KL_STATUS);	//Setup logging
	Log::log(KL_STATUS, "kmain : Melon booting.");

	Usr::load();			//Setup user managment
	Log::log(KL_STATUS, "kmain : User list loaded");

	if (init.empty()) {
		*kvt << "\n\n";
		new KernelShell(cwd, kvt);
		while (1) asm volatile("sti; hlt");
	} else {
		selectVideoMode(*kvt);
		//Create a VT for handling the Melon bootup logo
		SimpleVT *melonLogoVT = new SimpleVT(melonLogoLines, melonLogoCols, TXTLOGO_FGCOLOR, TXTLOGO_BGCOLOR);
		melonLogoVT->map(1);
		new Thread(logoAnimation, (void*)melonLogoVT, true);

		Process* p = Process::run(init, 0);
		if (p == 0) {
			PANIC((char*)(u8int*)ByteArray(String("Could not launch init : ") += init));
		} else {
			Log::log(KL_STATUS, String("kmain : Starting init : ") += init);
			VirtualTerminal* vt = new ScrollableVT(Disp::textRows() - 10, Disp::textCols() - 4,
					200, SHELL_FGCOLOR, SHELL_BGCOLOR);
			Kbd::setFocus(vt);
			((ScrollableVT*)vt)->map(9);
			p->setInVT(vt);
			p->setOutVT(vt);
			p->start();
			while (p->getState() != P_FINISHED) Task::currThread()->sleep(100);
			Sys::halt();
		}
	}

	PANIC("END OF KMAIN");
}
