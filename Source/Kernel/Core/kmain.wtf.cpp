//This file contains the kernel's main procedure

#include <Core/common.wtf.h>
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
#include <VTManager/PipeVT.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <MemoryManager/GDT.ns.h>
#include <TaskManager/Task.ns.h>
#include <SyscallManager/IDT.ns.h>
#include <Library/String.class.h>
#include <Library/ByteArray.class.h>
#include <VFS/Part.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>
#include <VFS/FileNode.class.h>
#include <VFS/VFS.ns.h>
#include <VFS/DirectoryNode.class.h>
#include <VFS/TextFile.class.h>
#include <Core/Log.ns.h>

#include <Ressources/Graphics/logo.text.cxd>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

#define INFO(vt) vt->setColor(KVT_FGCOLOR); *vt << " - "; vt->setColor(KVT_LIGHTCOLOR);
#define PROCESSING(vt, m) vt->setColor(KVT_BLECOLOR); *vt << " > "; vt->setColor(KVT_FGCOLOR); *vt << m; \
	vt->setCursorCol(60); vt->setColor(KVT_LIGHTCOLOR); *vt << ": ";
#define OK(vt) vt->setColor(KVT_FGCOLOR); *vt << "[ "; vt->setColor(KVT_OKCOLOR); *vt << "OK"; vt->setColor(KVT_FGCOLOR); *vt << " ]\n";

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
	for (int i = 0; i < melonLogoLines; i++) {
		for (int j = 0; j < melonLogoCols; j++) {
			melonLogoVT->putChar(i, j, melonLogo[i][j]);
		}
	}
	melonLogoVT->map(1);

	//Create a VT for logging what kernel does
	SimpleVT *kvt = new ScrollableVT(15, 76, 100, KVT_FGCOLOR, KVT_BGCOLOR);
	kvt->map(melonLogoLines + 2);

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
	Task::initialize(String((char*)mbd->cmdline), kvt); OK(kvt);

	PROCESSING(kvt, "Mounting first module as ramfs on root directory...");
	FileSystem* fs = new RamFS((u8int*)mods[0].mod_start, 1024 * 1024);
	DirectoryNode* cwd;
	cwd = fs->getRootNode();
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

	asm volatile("sti");

	while(1) {
		kvt->setColor(KVT_LIGHTCOLOR);
		*kvt << "[" << cwd->getName() << "]# ";
		kvt->setColor(KVT_ENTRYCOLOR);
		Vector<String> tokens = kvt->readLine().split(" ");
		kvt->setColor(KVT_FGCOLOR);
		if (tokens[0] == "help") {
			*kvt << " - Command list for integrated kernel shell:\n";
			*kvt << "  - help          shows this help screen\n";
			*kvt << "  - reboot        reboots your computer\n";
			*kvt << "  - halt          shuts down your computer\n";
			*kvt << "  - panic         causes a kernel panic\n";
			*kvt << "  - devices       shows all detected devices on your computer\n";
			*kvt << "  - loadkeys      loads specified kekymap\n";
			*kvt << "  - free          shows memory usage (physical frames and kernel heap)\n";
			*kvt << "  - uptime        shows seconds since boot\n";
			*kvt << "  - part          shows all detected block devices and partitions\n";
			*kvt << " - Commands you should know how to use : ls, cd, cat, pwd, rm, mkdir, wf\n";
		} else if (tokens[0] == "reboot") {
			Sys::reboot();
		} else if (tokens[0] == "halt") {
			Sys::halt();
		} else if (tokens[0] == "panic") {
			PANIC("This is what happens when you say 'panic'.");
		} else if (tokens[0] == "ls") {
			DirectoryNode* d = cwd;
			if (tokens.size() == 2) {
				FSNode* n = VFS::find(tokens[1], cwd);
				d = NULL;
				if (n == NULL)
					*kvt << "No such directory : " << tokens[1] << "\n";
				else if (n->type() != NT_DIRECTORY)
					*kvt << "Not a directory : " << tokens[1] << "\n";
				else
					d = (DirectoryNode*)n;
			}	
			if (d != NULL) *kvt << "Contents of directory " << VFS::path(d) << " :\n";
			for (u32int i = 0; d != NULL && i < d->getLength(); i++) {
				FSNode* n = d->getChild(i);
				if (n->type() == NT_FILE) {
					FileNode* f = (FileNode*)n;
					*kvt << " - FILE\t" << f->getName();
					kvt->setCursorCol(30);
					*kvt << (s32int)f->getLength() << " bytes.\n";
				} else if (n->type() == NT_DIRECTORY) {
					*kvt << " - DIR\t" << n->getName() << "/";
					kvt->setCursorCol(30);
					*kvt << (s32int)n->getLength() << " items.\n";
				}
			}
		} else if (tokens[0] == "cd") {
			if (tokens.size() == 1) {
				*kvt << "Error : no argument given.\n";
			} else {
				FSNode* n = VFS::find(tokens[1], cwd);
				if (n == NULL)
					*kvt << "No such directory : " << tokens[1] << "\n";
				else if (n->type() != NT_DIRECTORY)
					*kvt << "Not a directory : " << tokens[1] << "\n";
				else
					cwd = (DirectoryNode*)n;
			}
		} else if (tokens[0] == "cat") {
			if (tokens.size() == 1) *kvt << "Meow.\n";
			for (u32int i = 1; i < tokens.size(); i++) {
				File f(tokens[i], FM_READ, cwd);
				if (f.valid()) {
					u8int *buff = (u8int*)Mem::kalloc(f.length() + 1);
					f.read(f.length(), buff);
					buff[f.length()] = 0;
					*kvt << String((const char*) buff);
					Mem::kfree(buff);
				} else {
					*kvt << "Error reading from file " << tokens[i] << "\n";
				}
			}
		} else if (tokens[0] == "pwd") {
			*kvt << "Current location : " << VFS::path(cwd) << "\n";
		} else if (tokens[0] == "rm") {
			if (tokens.size() == 1) *kvt << "Error : no argument specified.\n";
			for (u32int i = 1; i < tokens.size(); i++) {
				if (!VFS::remove(tokens[i], cwd)) {
						*kvt << "Error while removing file " << tokens[i] << "\n";
					}
			}
		} else if (tokens[0] == "mkdir") {
			if (tokens.size() > 1) {
				for (u32int i = 1; i < tokens.size(); i++) {
					if (VFS::createDirectory(tokens[i], cwd) == NULL)
						*kvt << "Error while creating directory " << tokens[i] << "\n";
				}
			} else {
				*kvt << "No argument specified.\n";
			}
		} else if (tokens[0] == "wf") {
			if (tokens.size() == 1) {
				*kvt << "No file to write !\n";
			} else {
				File f(tokens[1], FM_TRUNCATE, cwd);
				if (f.valid()) {
					String t = kvt->readLine();
					while (t != ".") {
						t += "\n";
						ByteArray temp(t);
						f.write(temp);
						t = kvt->readLine();
					}
				} else {
					*kvt << "Error openning file.\n";
				}
			}
		} else if (tokens[0] == "devices") {
			Vector<Device*> dev = Dev::findDevices();
			*kvt << " - Detected devices :\n";
			for (u32int i = 0; i < dev.size(); i++) {
				*kvt << "  - " << dev[i]->getClass();
				kvt->setCursorCol(25);
				*kvt << dev[i]->getName() << "\n";
			}
		} else if (tokens[0] == "loadkeys") {
			if (tokens.size() == 1) {
				*kvt << "Error : no argument specified.\n";
			} else {
				if (!Kbd::loadKeymap(tokens[1])) {
					*kvt << "Error while loading keymap " << tokens[1] << ".\n";
				}
			}
		} else if (tokens[0] == "free") {
			u32int frames = PhysMem::total(), freef = PhysMem::free();
			*kvt << " - Free frames : " << (s32int)freef << " (" << (s32int)(freef * 4 / 1024) << "Mo) of "
			   	<< (s32int)frames << " (" << (s32int)(frames * 4 / 1024) << "Mo).\n";
			u32int kh = Mem::kheapSize(), freek = Mem::kheapFree;
			*kvt << " - Kernel heap free : " << (s32int)(freek / 1024 / 1024) << "Mo (" << (s32int)(freek / 1024) <<
				"Ko) of " << (s32int)(kh / 1024 / 1024) << "Mo (" << (s32int)(kh / 1024) << "Ko).\n";
		} else if (tokens[0] == "uptime") {
			*kvt << " - Uptime : " << (s32int)(Time::uptime()) << "s.\n";
		} else if (tokens[0] == "part") {
			*kvt << " *  ID\tClass                    Name\n";
			for (u32int i = 0; i < Part::devices.size(); i++) {
				*kvt << "  - " << (s32int)i << "\t";
				if (Part::devices[i] == 0) {
					*kvt << "[none]\n";
				} else {
					*kvt << Part::devices[i]->getClass();
					kvt->setCursorCol(33);
					*kvt << Part::devices[i]->getName() << "\n";
					for (u32int j = 0; j < Part::partitions.size(); j++) {
						if (Part::partitions[j]->getDevice() == Part::devices[i]) {
							*kvt << "\t   - Partition " << (s32int)Part::partitions[j]->getPartNumber() <<
								", start at " << (s32int)Part::partitions[j]->getStartBlock() <<
								", size " << (s32int)Part::partitions[j]->getBlockCount() << "\n";
						}
					}
				}
			}
		} else if (tokens.size() > 1 or tokens[0] != "") {
			*kvt << " - Unrecognized command: " << tokens[0] << "\n";
		}
	}
	PANIC("END OF KMAIN");
}
