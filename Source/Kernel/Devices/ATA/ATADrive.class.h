#ifndef DEF_ATADRIVE_CLASS_H
#define DEF_ATADRIVE_CLASS_H

#include "ATAController.class.h"
#include <Devices/BlockDevice.proto.h>

class ATADrive : public BlockDevice {
	friend class ATAController;
	private:
	ATADrive(ATAController* ctrlr, bool isSlave, u32int blockCount, u16int* identifyData);

	ATAController* m_ctrlr;
	bool m_isSlave;
	u32int m_blockCount;
	u16int m_identifyData[256];

	void cmdCommon(u32int numblock, u32int count);

	public:
	String getClass();
	String getName();

	bool readOnly();
	u64int blocks();
	bool readBlocks(u64int start, u32int count, u8int* data);
	bool writeBlocks(u64int start, u32int count, u8int* data);
	u32int blockSize();
};

#endif
