#ifndef DEF_PARTITION_CLASS_H
#define DEF_PARTITION_CLASS_H

#include <Devices/BlockDevice.proto.h>
#include <VFS/BlockCache.class.h>

class Partition {
	private:
	BlockDevice* m_device;
	BlockCache<BlockDevice> m_cache;
	u64int m_startblock, m_blockcount;
	u8int m_partnumber;	//Partition number in partition table of device

	public:
	Partition(BlockDevice* dev, u8int partnumber, u64int startblock, u64int blockcount);

	bool readBlocks(u64int startblock, u32int count, u8int *data);
	bool writeBlocks(u64int startblock, u32int count, u8int *data);

	//These two just use the readBlocks && writeBlocks defined above
	bool read(u64int start, u32int length, u8int *data);
	bool write(u64int start, u32int length, u8int *data);

	//Accessors :
	BlockDevice* getDevice();
	u64int getStartBlock();
	u64int getBlockCount();
	u8int getPartNumber();
	u32int getBlockSize();
	inline u64int blocks() { return getBlockCount(); }
};

#endif

