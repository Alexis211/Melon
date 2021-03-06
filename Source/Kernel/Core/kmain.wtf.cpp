//This file contains the kernel's main procedure

#include <common.h>
#include <Core/multiboot.wtf.h>

#include <Devices/Display/VGATextOutput.class.h>
#include <Devices/Display/VESADisplay.class.h>
#include <Devices/Keyboard/PS2Keyboard.class.h>
#include <Devices/Floppy/FloppyDrive.class.h>
#include <Devices/ATA/ATAController.class.h>
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
#include <FileSystems/FAT/FATFS.class.h>
#include <VFS/FileNode.class.h>
#include <VFS/VFS.ns.h>
#include <VFS/DirectoryNode.class.h>
#include <Core/Log.ns.h>
#include <Core/SB.ns.h>
#include <Shell/KernelShell.class.h>

#include <Map.class.h>

#include <Ressources/Graphics/logo.text.cxd>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

SimpleVT* kvt;

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

	SB::init();


	//Create a VT for logging what kernel does
	SB::progress("Create kernel VT");
	kvt = new ScrollableVT(24, 80, 20, KVT_FGCOLOR, KVT_BGCOLOR);
	kvt->map(1, 0);
	*kvt << MVT::setcolor(TXTLOGO_FGCOLOR, TXTLOGO_BGCOLOR);
	for (int i = 0; i < melonLogoLines; i++) {
		*kvt << MVT::setcsrcol(40 - (melonLogoCols / 2));
		*kvt << melonLogo[i] << "\n";
	}
	*kvt << MVT::setcolor(KVT_FGCOLOR, KVT_BGCOLOR);

	SB::progress("IDT");
	IDT::init();		//Setup interrupts 

	SB::progress("Paging");
	u32int totalRam = ((mbd->mem_upper + mbd->mem_lower) * 1024);
	PhysMem::initPaging(totalRam);		//Setup paging 

	SB::progress("GDT");
	GDT::init();		//Initialize real GDT, not fake one from loader.wtf.asm
	PhysMem::removeTemporaryPages(); 	//Remove useless page mapping

	SB::progress("Heap");
	Mem::createHeap();		//Create kernel heap 
	Dev::registerDevice(vgaout);

	SB::progress("Timer");
	Dev::registerDevice(new Timer()); 	//Initialize timer
	String kcmdline((char*)mbd->cmdline);

	SB::progress("Multitasking");
	Task::initialize(kcmdline, kvt);	//Initialize multitasking 
	SB::gomulti();

	asm volatile("sti");

	//***************************************	PARSE COMMAND LINE
	
	SB::progress("Parse command line");
	Vector<String> opts = kcmdline.split(" ");
	String keymap = "builtin", init = DEFAULT_INIT;
	String root = DEFAULT_ROOT;
	Vector<String> mount;
	bool enableVESA = DEFAULT_ENABLEVESA;
	for (u32int i = 0; i < opts.size(); i++) {
		Vector<String> opt = opts[i].split(":");
		if (opt[0] == "vesa" && opt[1] != "enabled") enableVESA = false;
		if (opt[0] == "keymap") keymap = opt[1];
		if (opt[0] == "init") init = opt[1];
		if (opt[0] == "root") root = opts[i].substr(5);
		if (opt[0] == "mount") mount.push(opts[i].substr(6));
	}

	//*************************************** 	DEVICE SETUP

	SB::progress("Keyboard");		Dev::registerDevice(new PS2Keyboard());	//Initialize keyboard driver
	Kbd::setFocus(kvt);	//Set focus to virtual terminal
	SB::progress("VESA");			if (enableVESA) Dev::registerDevice(new VESADisplay());
	SB::progress("Floppy");		FloppyController::detect();
	SB::progress("Hard disk drives");	ATAController::detect();

	//***************************************	MOUNT FILESYSTEMS

	SB::progress("Root filesystem");
	{	// mount root filesystem
		if (!VFS::mount(String("/:") += root, kvt, mbd)) PANIC("Cannot mount root filesystem.");
	}
	DirectoryNode* cwd;
	cwd = VFS::getRootNode();
	Task::currProcess()->setCwd(cwd);

	SB::progress("File systems");
	// mount other filesystems
	for (u32int i = 0; i < mount.size(); i++) {
		VFS::mount(mount[i], kvt, mbd);
	}

	{
		TextFile mounts("/System/Configuration/Mount", FM_READ);
		while (mounts.valid() && !mounts.eof()) {
			String m = mounts.readLine();
			if (!m.empty() && m[0] != WChar("#")) VFS::mount(m, kvt, mbd);
		}
	}
	
	//***************************************	LOAD SYSTEM STUFF

	SB::progress("Logging");
	Log::init(KL_STATUS);	//Setup logging
	Log::log(KL_STATUS, "kmain : Melon booting.");

	if (keymap != "builtin") {
		SB::progress("Keymap");
		if (!Kbd::loadKeymap(keymap)) Log::log(KL_WARNING, String("WARNING : Could not load keymap ") += keymap += ", using built-in keymap instead.");
	}

	SB::progress("Users");
	Usr::load();			//Setup user managment
	Log::log(KL_STATUS, "kmain : User list loaded");

	SB::progress("Video mode selection");
	Disp::selectMode();
	if (init.empty()) {
		SB::progress("Start kernel shell");
		*kvt << "\n";
		new KernelShell(cwd, kvt);
		SB::message("Melon is running");
		while (KernelShell::getInstances() > 0) {
			Task::currThread()->sleep(100);
		}
		Sys::halt();
	} else {
		SB::progress("Launch INIT");
		Process* p = Process::run(init, 0);
		if (p == 0) {
			PANIC((char*)(u8int*)ByteArray(String("Could not launch init : ") += init));
		} else {
			Log::log(KL_STATUS, String("kmain : Starting init : ") += init);
			p->setInVT(kvt);
			p->setOutVT(kvt);
			p->start();
			SB::message("Init started");
			while (p->getState() != P_FINISHED) Task::currThread()->sleep(100);
			PANIC("Init has terminated");
		}
	}

	PANIC("END OF KMAIN");
}
