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

//Accessors
BlockDevice* Partition::getDevice() { return m_device; }
u64int Partition::getStartBlock() { return m_startblock; }
u64int Partition::getBlockCount() { return m_blockcount; }
u8int Partition::getPartNumber() { return m_partnumber; }
u32int Partition::getBlockSize() { return m_device->blockSize(); }
