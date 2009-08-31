#ifndef DEF_FLOPPYDRIVE_CLASS_H
#define DEF_FLOPPYDRIVE_CLASS_H

#include <Devices/BlockDevice.proto.h>

#include "FloppyController.class.h"

class FloppyDrive : public BlockDevice {
	friend class FloppyController;
	friend u32int floppyMotorTimer();
	private:
	FloppyDrive(FloppyController *fdc, u8int number, u8int type);	//Private constructor, called by FloppyController()

	u8int m_motorState, m_motorTimeout, m_driveNumber, m_driveType;
	u32int m_cylinders, m_sectors;
	FloppyController *m_fdc;

	u8int m_buffer[FLOPPY_DMALEN];
	u32int m_buffCyl, m_buffTime;	//Used for keeping track of what is in buffer, this is a sort of cache system

	bool setup();
	bool calibrate();
	bool setMotorState(bool on);
	bool killMotor();
	bool seek(u32int cyli, s32int head);
	bool doTrack(u32int cyl, u8int dir);

	public:
	String getClass();
	String getName();

	bool readOnly();
	u64int blocks();
	bool readBlocks(u64int startblock, u32int count, u8int *data);
	bool writeBlocks(u64int startblock, u32int count, u8int* data);
	u32int blockSize();	//Always 512

	u64int chs2lba(u32int cylinder, u32int head, u32int sector);
};

#endif
