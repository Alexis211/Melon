#ifndef DEF_PART_NS_H
#define DEF_PART_NS_H

#include <Devices/BlockDevice.proto.h>
#include <Vector.class.h>
#include <VFS/Partition.class.h>

namespace Part {
	extern Vector<BlockDevice*> devices;
	extern Vector<Partition*> partitions;

	void registerDevice(BlockDevice* dev);
	void unregisterDevice(BlockDevice* dev);

	u32int getDeviceID(BlockDevice* dev);
}

#endif
