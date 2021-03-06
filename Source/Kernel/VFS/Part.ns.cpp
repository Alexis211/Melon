#include "Part.ns.h"

#include <VTManager/SimpleVT.class.h>

namespace Part {

Vector<BlockDevice*> devices;
Vector<Partition*> partitions;

void readPartitionTable(BlockDevice *dev) {	//TODO : read partition table from device
	partitions.push(new Partition(dev, 0, 0, dev->blocks()));	//Insert whole device as a partition

	u8int* mbr = (u8int*)Mem::alloc(dev->blockSize());
	if (!dev->readBlocks(0, 1, mbr)) return;

	mbr_entry_t* entries = (mbr_entry_t*)((u32int)mbr + 0x1BE);

	for (u32int i = 0; i < 4; i++) {
		if ((entries[i].bootable == 0 or entries[i].bootable == 0x80) and entries[i].id != 0
			   	and entries[i].s_lba != 0 and entries[i].size != 0
				and entries[i].s_lba < dev->blocks() and entries[i].size < dev->blocks()) {
			partitions.push(new Partition(dev, i + 1, entries[i].s_lba, entries[i].size));
		}
	}

	Mem::free(mbr);
}

void registerDevice(BlockDevice *dev) {
	unregisterDevice(dev);

	asm volatile("cli");

	readPartitionTable(dev);

	bool inserted = false;
	for (u32int i = 0; i < devices.size(); i++) {
		if (devices[i] == 0) {
			devices[i] = dev;
			inserted = true;
			break;
		}
	}
	if (!inserted) devices.push(dev);

	asm volatile("sti");
}

void unregisterDevice(BlockDevice *dev) {
	asm volatile("cli");
	//Unregister && delete partitions
	for (u32int i = 0; i < partitions.size(); i++) {
		if (partitions[i]->getDevice() == dev) {
			delete partitions[i];
			partitions[i] = partitions.back();
			partitions.pop();
			i--;
		}
	}
	//Unregister device
	for (u32int i = 0; i < devices.size(); i++) {
		if (devices[i] == dev) devices[i] = 0;
	}
	if (!devices.empty() && devices.back() == 0) devices.pop();

	asm volatile("sti");
}

u32int getDeviceID(BlockDevice* dev) {
	for (u32int i = 0; i < devices.size(); i++) {
		if (devices[i] == dev) return i;
	}
	return (u32int) - 1;
}

BlockDevice* dev(String _class, u32int idx) {
	if (_class.empty()) return devices[idx];
	for (u32int i = 0; i < devices.size(); i++) {
		String devclass = devices[i]->getClass();
		if (devclass == _class or (devclass.size() > _class.size() and devclass.substr(0, _class.size()) == _class)) {
			if (idx == 0) {
				return devices[i];
			} else {
				idx--;
			}
		}
	}
	return NULL;
}

Partition* part(BlockDevice* dev, u32int idx) {
	for (u32int i = 0; i < partitions.size(); i++) {
		if (partitions[i]->getDevice() == dev && partitions[i]->getPartNumber() == idx) {
			return partitions[i];
		}
	}
	return NULL;
}

String partIdentifier(Partition* p) {
	return String("d") += String::number(getDeviceID(p->getDevice())) += String("p") += String::number(p->getPartNumber());
}

}
