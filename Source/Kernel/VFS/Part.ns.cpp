#include "Part.ns.h"

namespace Part {

Vector<BlockDevice*> devices;
Vector<Partition*> partitions;

void readPartitionTable(BlockDevice *dev) {	//TODO : read partition table from device
	partitions.push(new Partition(dev, 0, 0, dev->blocks()));	//Insert whole device as a partition
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

}
