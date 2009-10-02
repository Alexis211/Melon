#include "KernelShell.class.h"
#include <DeviceManager/Dev.ns.h>
#include <DeviceManager/Time.ns.h>
#include <MemoryManager/Mem.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/Part.ns.h>

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
	u32int kh = Mem::kheapSize(), freek = Mem::kheapFree;
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
