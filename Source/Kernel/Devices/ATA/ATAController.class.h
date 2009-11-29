#ifndef DEF_ATACONTROLLER_CLASS_H
#define DEF_ATACONTROLLER_CLASS_H

#include <Devices/Device.proto.h>

#define ATA_BUS1_BASE 0x1F0
#define ATA_BUS2_BASE 0x170

#define ATA_PORT_DATA 0
#define ATA_PORT_FEATURES_ERROR 1
#define ATA_PORT_SECT_COUNT 2
#define ATA_PORT_PARTIAL1 3
#define ATA_PORT_PARTIAL2 4
#define ATA_PORT_PARTIAL3 5
#define ATA_PORT_DRIVE_SELECT 6
#define ATA_PORT_COMMAND 7

#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30

class ATADrive;

class ATAController : public Device {
	friend class ATADrive;
	private:
	ATAController(u32int base, u8int number);

	u32int m_base;
	u8int m_number;

	ATADrive* m_drives[2];

	void writeWord(u16int port, u16int word) { Sys::outw(m_base + port, word); }
	void writeByte(u16int port, u8int byte) { Sys::outb(m_base + port, byte); }
	u8int readByte(u16int port) { return Sys::inb(m_base + port); }
	u16int readWord(u16int port) { return Sys::inw(m_base + port); }

	void identify(bool slave);	//Identifies a drive and adds it to m_drives

	public:
	static void detect();
	String getClass();
	String getName();
};

#endif
