#include "KernelShell.class.h"
#include <DeviceManager/Dev.ns.h>
#include <DeviceManager/Time.ns.h>
#include <MemoryManager/Mem.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/Part.ns.h>
#include <VFS/VFS.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>

void KernelShell::devices(Vector<String>& args) {
	Vector<Device*> dev = Dev::findDevices();
	*m_vt << " - Detected devices :\n";
	for (u32int i = 0; i < dev.size(); i++) {
		*m_vt << "  - " << dev[i]->getClass();
		m_vt->setCursorCol(25);
		*m_vt << dev[i]->getName() << "\n";
	}
}

void KernelShell::loadkeys(Vector<String>& args) {
	if (args.size() == 1) {
		*m_vt << "Error : no argument specified.\n";
	} else {
		if (!Kbd::loadKeymap(args[1])) {
			*m_vt << "Error while loading keymap " << args[1] << ".\n";
		}
	}
}

void KernelShell::free(Vector<String>& args) {
	u32int frames = PhysMem::total(), freef = PhysMem::free();
	*m_vt << " - Free frames : " << (s32int)freef << " (" << (s32int)(freef * 4 / 1024) << "Mo) of "
		<< (s32int)frames << " (" << (s32int)(frames * 4 / 1024) << "Mo).\n";
	u32int kh = Mem::kheapSize(), freek = Mem::kheapFree();
	*m_vt << " - Kernel heap free : " << (s32int)(freek / 1024 / 1024) << "Mo (" << (s32int)(freek / 1024) <<
		"Ko) of " << (s32int)(kh / 1024 / 1024) << "Mo (" << (s32int)(kh / 1024) << "Ko).\n";
}

void KernelShell::uptime(Vector<String>& args) {
		*m_vt << " - Uptime : " << (s32int)(Time::uptime()) << "s.\n";
}

void KernelShell::part(Vector<String>& args) {
	*m_vt << " *  ID\tClass                    Name\n";
	for (u32int i = 0; i < Part::devices.size(); i++) {
		*m_vt << "  - " << (s32int)i << "\t";
		if (Part::devices[i] == 0) {
			*m_vt << "[none]\n";
		} else {
			*m_vt << Part::devices[i]->getClass();
			m_vt->setCursorCol(33);
			*m_vt << Part::devices[i]->getName() << "\n";
			for (u32int j = 0; j < Part::partitions.size(); j++) {
				if (Part::partitions[j]->getDevice() == Part::devices[i]) {
					*m_vt << "\t   - Partition " << (s32int)Part::partitions[j]->getPartNumber() <<
						", start at " << (s32int)Part::partitions[j]->getStartBlock() <<
						", size " << (s32int)Part::partitions[j]->getBlockCount() << "\n";
				}
			}
		}
	}
}

void KernelShell::mount(Vector<String>& args) {
	if (args.size() == 1) {
		for (u32int i = 0; i < VFS::filesystems.size(); i++) {
			*m_vt << VFS::filesystems[i]->getIdentifier() << "\n"; 
		}
	} else if (args.size() == 2) {
		if (args[1] == "help") {
			*m_vt << "Usage: mount [help|<options>]\n" <<
				"Options formats :\n" << 
				" - <mountpoint>:ramfs\n" <<
				" - <mountpoint>:[<dev_class>]:<dev_number>:<part_number>[:<type>[:[ro|rw]]]\n" <<
				"You can have a list of available block devices and partitions by typing 'part'.\n";
		} else {
			if (VFS::mount(args[1], m_vt)) *m_vt << "Ok, filesystem mounted.\n";
		}
	} else {
		*m_vt << "Wrong options. Type 'mount help' for more info.\n";
	}
}

void KernelShell::unmount(Vector<String>& args) {
	if (args.size() == 2) {
		FSNode* n = VFS::find(args[1], m_cwd);
		bool ok = false;
		if (n == 0) {
			ok = false;
		} else {
			String p = VFS::path(n);
			for (u32int i = 0; i < VFS::filesystems.size(); i++) {
				if (VFS::path(VFS::filesystems[i]->getRootNode()) == p) {
					ok = VFS::unmount(VFS::filesystems[i]);
					break;
				}
			}
		}
		if (ok) *m_vt << "Ok, filesystem unmounted.\n";
		else *m_vt << "Error.\n";
	} else {
		*m_vt << "Usage: unmount <mountpoint>\n";
	}
}

void KernelShell::readblock(Vector<String>& args) {
	if (args.size() == 3) {
		Vector<Device*> devcs = Dev::findDevices("block");
		u32int id = args[1].toInt(), block = args[2].toInt();
		if (id < devcs.size()) {
			BlockDevice* bdev = (BlockDevice*)devcs[id];
			*m_vt << "Block " << block << " from device " << bdev->getName() << " (" << bdev->getClass() << ")\n";
			u8int* buff = (u8int*)Mem::alloc(bdev->blockSize());
			bdev->readBlocks(block, 1, buff);
			m_vt->hexDump(buff, 32);
			Mem::free(buff);
		} else {
			*m_vt << "Block device #" << id << " does not exist.\n";
		}
	} else {
		*m_vt << "Usage: readblock <dev id> <block id>\n";
	}
}
