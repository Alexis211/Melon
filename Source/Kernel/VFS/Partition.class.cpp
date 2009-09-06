#include "Partition.class.h"

Partition::Partition(BlockDevice* dev, u8int partnumber, u64int startblock, u64int blockcount) {
	m_device = dev;
	m_partnumber = partnumber;
	m_startblock = startblock;
	m_blockcount = blockcount;
}

bool Partition::readBlocks(u64int startblock, u32int count, u8int *data) {
	if (startblock + count > m_startblock + m_blockcount) return false;
	return m_device->readBlocks(startblock - m_startblock, count, data);
}

bool Partition::writeBlocks(u64int startblock, u32int count, u8int *data) {
	if (startblock + count > m_startblock + m_blockcount) return false;
	return m_device->writeBlocks(startblock - m_startblock, count, data);
}

bool Partition::read(u64int start, u32int length, u8int *data) {
	u32int blksz = m_device->blockSize();
	u64int startBlock = start / blksz;
	u32int offset = start % blksz;
	u32int blocks = (length + offset) / blksz;
	if ((length + offset) % blksz != 0) blocks++;

	u8int* buff = new u8int [blocks * blksz];	//THIS MAY FAIL :D
	if (buff == 0) return false;

	if (!readBlocks(startBlock, blocks, buff)) return false;
	memcpy(data, buff + offset, length);

	delete [] buff;
	return true;
}

bool Partition::write(u64int start, u32int length, u8int *data) {
	u32int blksz = m_device->blockSize();
	u64int startBlock = start / blksz;
	u32int offset = start % blksz;
	u32int blocks = (length + offset) / blksz;
	if ((length + offset) % blksz != 0) blocks++;
	u64int lastBlock = startBlock + (u64int)blocks - 1;

	u8int* buff = new u8int [blocks * blksz];
	if (buff == 0) return false;

	if (offset != 0) {
		if (!readBlocks(startBlock, 1, buff)) return false;
	}
	if (lastBlock != startBlock and (length + offset) % blksz != 0) {
		if (!readBlocks(lastBlock, 1, buff + ((blocks - 1) * blksize))) return false;
	}
	memcpy(buff + offset, data, length);
	if (!writeBlocks(startBlock, blocks, buff)) return false;

	delete[] buff;
	return true;
}

//Accessors
BlockDevice* Partition::getDevice() { return m_device; }
u64int Partition::getStartBlock() { return m_startblock; }
u64int Partition::getBlockCount() { return m_blockcount; }
u8int Partition::getPartNumber() { return m_partnumber; }
u32int Partition::getBlockSize() { return m_device->blockSize(); }
