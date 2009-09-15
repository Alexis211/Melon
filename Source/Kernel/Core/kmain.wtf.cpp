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
#include <VTManager/VirtualTerminal.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <MemoryManager/PageAlloc.ns.h>
#include <MemoryManager/GDT.ns.h>
#include <TaskManager/Task.ns.h>
#include <SyscallManager/IDT.ns.h>
#include <Library/String.class.h>
#include <VFS/Part.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>
#include <VFS/FileNode.class.h>
#include <VFS/VFS.ns.h>
#include <VFS/DirectoryNode.class.h>

#include <Ressources/logo.cd>
#include <Ressources/keymap-fr.wtf.c>

extern u32int end;	//Placement address

extern "C" void kmain(multiboot_info_t* mbd, u32int magic);

#define INFO(vt) vt->setColor(0); *vt << " - "; vt->setColor(8);
#define PROCESSING(vt, m) vt->setColor(6); *vt << " > "; vt->setColor(0); *vt << m; \
	vt->setCursorCol(60); vt->setColor(8); *vt << ": ";
#define OK(vt) vt->setColor(0); *vt << "[ "; vt->setColor(1); *vt << "OK"; vt->setColor(0); *vt << " ]\n";

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
	VirtualTerminal *melonLogoVT = new VirtualTerminal(melonLogoLines, melonLogoCols, 9, 0);
	for (int i = 0; i < melonLogoLines; i++) {
		for (int j = 0; j < melonLogoCols; j++) {
			melonLogoVT->putChar(i, j, melonLogo[i][j]);
		}
	}
	melonLogoVT->map(1);

	//Create a VT for logging what kernel does
	VirtualTerminal *kvt = new VirtualTerminal(15, 76, 0, 7);
	kvt->map(melonLogoLines + 2);

	INFO(kvt); *kvt << "Lower ram : " << (s32int)mbd->mem_lower << "k, upper : " << (s32int)mbd->mem_upper << "k.\n";
	INFO(kvt); *kvt << "Placement address : " << (u32int)Mem::placementAddress << "\n";

	PROCESSING(kvt, "Loading IDT...");
	IDT::init(); OK(kvt);

	PROCESSING(kvt, "Initializing paging...");
	u32int totalRam = ((mbd->mem_upper + 1024) * 1024);
	PhysMem::initPaging(totalRam); OK(kvt);

	INFO(kvt); *kvt << "Total ram : " << (s32int)(totalRam / 1024) << "k (" << (s32int)(totalRam / (1024 * 1024)) << "M).\n";
	PROCESSING(kvt, "Initializing real GDT and cleaning page directory...");
	GDT::init();
	PhysMem::removeTemporaryPages(); OK(kvt);

	PROCESSING(kvt, "Creating kernel heap...");
	Mem::createHeap(); OK(kvt);
	INFO(kvt); *kvt << "Free frames : " << (s32int)PhysMem::free() << "/" << (s32int)PhysMem::total() << "\n";
	
	PROCESSING(kvt, "Registering textual VGA output...");
	Dev::registerDevice(vgaout); OK(kvt);

	PROCESSING(kvt,"Initializing PIT...");
	Dev::registerDevice(new Timer()); OK(kvt);

	PROCESSING(kvt, "Initializing multitasking...");
	Task::initialize(String((char*)mbd->cmdline), kvt); OK(kvt);

	PROCESSING(kvt, "Setting up keyboard...");
	Dev::registerDevice(new PS2Keyboard());	//Initialize keyboard driver
	Kbd::setKeymap(keymapFR_normal, keymapFR_shift, keymapFR_caps, keymapFR_altgr, keymapFR_shiftaltgr);	//Load keymap
	Kbd::setFocus(kvt);	//Set focus to virtual terminal
	OK(kvt);

	PROCESSING(kvt, "Detecting floppy drives...");
	FloppyController::detect(); OK(kvt);

	PROCESSING(kvt, "Mounting first module as ramfs on root directory...");
	FileSystem* fs = new RamFS((u8int*)mods[0].mod_start, 1024 * 1024);
	DirectoryNode* cwd;
	cwd = fs->getRootNode();
	VFS::setRootNode(cwd); OK(kvt);

	asm volatile("sti");

	while(1) {
		kvt->setColor(8);
		*kvt << "[" << cwd->getName() << "]# ";
		kvt->setColor(1);
		Vector<String> tokens = kvt->readLine().split(" ");
		kvt->setColor(0);
		if (tokens[0] == "help") {
			*kvt << " - Command list for integrated kernel shell:\n";
			*kvt << "  - help          shows this help screen\n";
			*kvt << "  - reboot        reboots your computer\n";
			*kvt << "  - devices       shows all detected devices on your computer\n";
			*kvt << "  - free          shows memory usage (physical frames and kernel heap)\n";
			*kvt << "  - uptime        shows seconds since boot\n";
			*kvt << "  - part          shows all detected block devices and partitions\n";
			*kvt << " - Commands you should know how to use : ls, cd, cat, pwd, rm, mkdir, wf\n";
		} else if (tokens[0] == "reboot") {
			Sys::reboot();
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
				FSNode* n = VFS::find(tokens[i], cwd);
				if (n == NULL) {
					*kvt << "No such file : " << tokens[i] << "\n";
				} else if (n->type() != NT_FILE) {
					*kvt << "Not a file : " << tokens[i]  << "\n";
				} else {
					FileNode* f = (FileNode*) n;
					u8int *buff = (u8int*)Mem::kalloc(f->getLength() + 1);
					f->read(0, f->getLength(), buff);
					buff[f->getLength()] = 0;
					*kvt << String((const char*) buff);
					Mem::kfree(buff);
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
			*kvt << "Sorry, this command isn't implemented yet.\n";
		} else if (tokens[0] == "devices") {
			Vector<Device*> dev = Dev::findDevices();
			*kvt << " - Detected devices :\n";
			for (u32int i = 0; i < dev.size(); i++) {
				*kvt << "  - " << dev[i]->getClass();
				kvt->setCursorCol(25);
				*kvt << dev[i]->getName() << "\n";
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
