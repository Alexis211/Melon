#include "ATADrive.class.h"

String ATADrive::getClass() {
	return "block.ata";
}

String ATADrive::getName() {
	return String("ATA drive ") += String::number(m_ctrlr->m_number) += String(m_isSlave ? ".slave " : ".master ")
		+= String::number(m_blockCount / (1024 * 1024 / blockSize())) += "MB";
}

ATADrive::ATADrive(ATAController* ctrlr, bool isSlave, u32int blockCount, u16int* identifyData) {
	m_ctrlr = ctrlr;
	m_isSlave = isSlave;
	m_blockCount = blockCount;
	for (u32int i = 0; i < 256; i++) m_identifyData[i] = identifyData[i];
}

bool ATADrive::readOnly() {
	return true;
}

u64int ATADrive::blocks() {
	return m_blockCount;
}

void ATADrive::cmdCommon(u32int numblock, u32int count) {
	m_ctrlr->writeByte(ATA_PORT_FEATURES_ERROR, 0);
	m_ctrlr->writeByte(ATA_PORT_SECT_COUNT, count);
	m_ctrlr->writeByte(ATA_PORT_PARTIAL1, numblock);
	m_ctrlr->writeByte(ATA_PORT_PARTIAL2, numblock >> 8);
	m_ctrlr->writeByte(ATA_PORT_PARTIAL3, numblock >> 16);

	m_ctrlr->writeByte(ATA_PORT_DRIVE_SELECT, 0xE0 | (m_isSlave ? 0x10 : 0) | ((numblock >> 24) & 0x0F));
}

bool ATADrive::readBlocks(u64int start, u32int count, u8int* data) {
	if (start + count >= m_blockCount) return false;

	m_ctrlr->waitLock();
	cmdCommon(start, count);
	m_ctrlr->writeByte(ATA_PORT_COMMAND, ATA_CMD_READ);

	while (!(m_ctrlr->readByte(ATA_PORT_COMMAND) & 0x08));	//Wait for the drive to be ready

	for (u32int idx = 0; idx < count * 256; idx++) {
		u16int tmpword = m_ctrlr->readWord(ATA_PORT_DATA);
		data[idx * 2] = (u8int)tmpword;
		data[idx * 2 + 1] = (u8int)(tmpword >> 8);
	}
	m_ctrlr->unlock();
	return true;	
}

bool ATADrive::writeBlocks(u64int start, u32int count, u8int* data) {
	if (start + count >= m_blockCount) return false;
	if (readOnly()) return false;

	m_ctrlr->waitLock();
	cmdCommon(start, count);
	m_ctrlr->writeByte(ATA_PORT_COMMAND, ATA_CMD_WRITE);

	while (!(m_ctrlr->readByte(ATA_PORT_COMMAND) & 0x08));	//Wait for the drive to be ready

	for (u32int idx = 0; idx < count * 256; idx++) {
		u16int tmpword = (data[idx * 2]) | (data[idx * 2 + 1] << 8); 
		m_ctrlr->writeByte(ATA_PORT_DATA, tmpword);
	}
	m_ctrlr->unlock();
	return true;	
}

u32int ATADrive::blockSize() {
	return 512;
}
