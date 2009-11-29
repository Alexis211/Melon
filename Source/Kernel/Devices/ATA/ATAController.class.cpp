#include "ATADrive.class.h"

#include <DeviceManager/Dev.ns.h>
#include <VFS/Part.ns.h>

String ATAController::getClass() {
	return "controller.ata";
}

String ATAController::getName() {
	return String("ATA controller #") + String::number(m_number);
}

void ATAController::detect() {
	ATAController *c[2];
	c[0] = new ATAController(ATA_BUS1_BASE, 0);
	c[1] = new ATAController(ATA_BUS2_BASE, 1);
	Dev::registerDevice(c[0]); Dev::registerDevice(c[1]);
	for (u32int d = 0; d < 2; d++) {
		for (u32int r = 0; r < 2; r++) {
			ATADrive* drv = c[d]->m_drives[r];
			if (drv != 0) {
				Dev::registerDevice(drv);
				Part::registerDevice(drv);
			}
		}
	}
}

ATAController::ATAController(u32int base, u8int number) {
	m_base = base; m_number = number;
	m_drives[0] = NULL; m_drives[1] = NULL;
	identify(false);	//Identify master device
	identify(true);		//Identify slave device
}

void ATAController::identify(bool slave) {
	if (m_drives[(slave ? 1 : 0)] != NULL) return;
	writeByte(ATA_PORT_DRIVE_SELECT, (slave ? 0xB0 : 0xA0)); 
	writeByte(ATA_PORT_COMMAND, ATA_CMD_IDENTIFY);
	u8int ret = readByte(ATA_PORT_COMMAND);
	if (ret == 0) return;	//Drive does not exist
	while ((ret & 0x88) != 0x08 and (ret & 1) != 1) {
		ret = readByte(ATA_PORT_COMMAND);
	}
	if ((ret & 1) == 1) return;	//Error while IDENTIFY
	u16int data[256];
	for (u32int i = 0; i < 256; i++) data[i] = readWord(ATA_PORT_DATA);
	u32int blocks = (data[61] << 16) | data[60];
	if (blocks != 0) {	//Drive supports LBA28
		m_drives[(slave ? 1 : 0)] = new ATADrive(this, slave, blocks, data);
	}
}
