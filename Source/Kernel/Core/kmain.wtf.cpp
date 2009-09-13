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

	FileSystem* fs = new RamFS((u8int*)mods[0].mod_start, 1024 * 1024);
	DirectoryNode* rd;
	rd = fs->getRootNode();
	/*FileNode* f;
	f = rd->createFile(String("test"));
	f->write(0, 4, (u8int*)"plop"); */

	asm volatile("sti");

	while(1) {
		kvt->setColor(0);
		*kvt << "> ";
		kvt->setColor(8);
		String tmp = kvt->readLine();
		kvt->setColor(0);
		if (tmp == "help") {
			*kvt << " - Command list for integrated kernel shell:\n";
			*kvt << "  - help          shows this help screen\n";
			*kvt << "  - reboot        reboots your computer\n";
			*kvt << "  - devices       shows all detected devices on your computer\n";
			*kvt << "  - free          shows memory usage (physical frames and kernel heap)\n";
			*kvt << "  - uptime        shows seconds since boot\n";
			*kvt << "  - part          shows all detected block devices and partitions\n";
		} else if (tmp == "ls") {
			for (u32int i = 0; i < rd->getLength(); i++) {
				FSNode* n = rd->getChild(i);
				if (n->type() == NT_FILE) {
					FileNode* f = (FileNode*)n;
					*kvt << "Found file " << f->getName() << ", length " << (s32int)f->getLength() << " :\n";
					u8int* d = (u8int*)Mem::kalloc(f->getLength() + 1);
					f->read(0, f->getLength(), d);
					d[f->getLength()] = 0;
					*kvt << String((const char*)d);
					Mem::kfree(d);
					*kvt << "\n";
				} else if (n->type() == NT_DIRECTORY) {
					*kvt << "Found directory " << n->getName() << ", " << (s32int)n->getLength() << " items.\n";
				}
			}
		} else if (tmp == "reboot") {
			Sys::reboot();
		} else if (tmp == "devices") {
			Vector<Device*> dev = Dev::findDevices();
			*kvt << " - Detected devices :\n";
			for (u32int i = 0; i < dev.size(); i++) {
				*kvt << "  - " << dev[i]->getClass();
				kvt->setCursorCol(25);
				*kvt << dev[i]->getName() << "\n";
			}
		} else if (tmp == "free") {
			u32int frames = PhysMem::total(), freef = PhysMem::free();
			*kvt << " - Free frames : " << (s32int)freef << " (" << (s32int)(freef * 4 / 1024) << "Mo) of "
			   	<< (s32int)frames << " (" << (s32int)(frames * 4 / 1024) << "Mo).\n";
			u32int kh = Mem::kheapSize(), freek = Mem::kheapFree;
			*kvt << " - Kernel heap free : " << (s32int)(freek / 1024 / 1024) << "Mo (" << (s32int)(freek / 1024) <<
				"Ko) of " << (s32int)(kh / 1024 / 1024) << "Mo (" << (s32int)(kh / 1024) << "Ko).\n";
		} else if (tmp == "uptime") {
			*kvt << " - Uptime : " << (s32int)(Time::uptime()) << "s.\n";
		} else if (tmp == "part") {
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
		} else if (!tmp.empty()) {
			*kvt << " - Unrecognized command: " << tmp << "\n";
		}
	}
	PANIC("END OF KMAIN");
}
