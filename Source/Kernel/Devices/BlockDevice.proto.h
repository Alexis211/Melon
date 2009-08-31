#ifndef DEF_BLOCKDEVICE_PROTO_H
#define DEF_BLOCKDEVICE_PROTO_H

#include <Devices/Device.proto.h>

class BlockDevice : public Device {
	public:
	virtual bool readOnly() = 0;
	virtual u64int blocks() = 0;	//Returns number of blocks on device
	virtual bool readBlocks(u64int startblock, u32int count, u8int *data) = 0;	//Returns false if fails
	virtual bool writeBlocks(u64int startblock, u32int count, u8int *data) = 0;	//Returns false if fails
	virtual u32int blockSize() = 0;	//Usually 512, 2048 for CDROMs

	virtual u64int chsToLBA(u32int cylinder, u32int head, u32int sector) { return 0; }	//Can be used by partition manager
};

#endif
