#ifndef DEF_PART_NS_H
#define DEF_PART_NS_H

#include <Devices/BlockDevice.proto.h>
#include <Vector.class.h>
#include <VFS/Partition.class.h>

namespace Part {
	struct mbr_entry_t {
			u8int   bootable;       /* 0 = no, 0x80 = bootable */
			u8int   s_head;         /* Starting head */
			u16int  s_sector : 6;   /* Starting sector */
			u16int  s_cyl    : 10;  /* Starting cylinder */
			u8int   id;             /* System ID */
			u8int   e_head;         /* Ending head */
			u16int  e_sector : 6;   /* Ending sector */
			u16int  e_cyl    : 10;  /* Ending cylinder */
			u32int  s_lba;          /* Starting sector (LBA value) */
			u32int  size;           /* Total sector number */
	} __attribute__ ((packed));

	extern Vector<BlockDevice*> devices;
	extern Vector<Partition*> partitions;

	void registerDevice(BlockDevice* dev);
	void unregisterDevice(BlockDevice* dev);

	u32int getDeviceID(BlockDevice* dev);

	BlockDevice* dev(String _class, u32int idx);
	Partition* part(BlockDevice* dev, u32int idx);

	String partIdentifier(Partition* p);	//Simply to help recognize the partition
}

#endif
