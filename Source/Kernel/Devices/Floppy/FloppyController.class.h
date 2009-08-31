#ifndef DEF_FLOPPYCONTROLLER_CLASS_H
#define DEF_FLOPPYCONTROLLER_CLASS_H

#include <Devices/Device.proto.h>
#include <TaskManager/Mutex.class.h>

#define FLOPPY_DMALEN 0x4800	//This is one cylinder

//Floppy registers
#define FR_DOR 2		//Digital Output Register
#define FR_MSR 4		//Main Status Register
#define FR_FIFO 5
#define FR_CCR 7		//Configuration Control Register

//Floppy commands
#define FC_SPECIFY 3
#define FC_SENSE_DRIVE_STATUS 4
#define FC_WRITE_DATA 5
#define FC_READ_DATA 6
#define FC_RECALIBRATE 7
#define FC_SENSE_INTERRUPT 8
#define FC_SEEK 15

//Floppy drives types
#define FT_NONE 0
#define FT_360K525 1		//360kB 5.25in
#define FT_12M525 2			//1.2MB 5.25in
#define FT_720K35 3			//720kB 3.5in
#define FT_144M35 4			//1.44MB 3.5in
#define FT_288M35 5			//2.88MB 3.5in

//Floppy drive motor states
#define FS_MOTOROFF 0
#define FS_MOTORON 1
#define FS_MOTORWAIT 2

//Floppy transfer directions
#define FD_READ 1
#define FD_WRITE 2

class FloppyDrive;

class FloppyController : public Device {
	friend class FloppyDrive;
	private:
	//For DMA
	static Mutex dmaMutex;
	static u8int dmabuff[FLOPPY_DMALEN];
	static bool dmaInit(u8int direction, u32int length);	//Locks the DMA Mutex. Called on each r/w operation.
	static void dmaRelease();	//Unlocks the DMA Mutex

	//For the FDC
	FloppyController(u32int base, u8int irq);	//Private constructor, called by Detect();

	u8int m_activeDrive;
	Mutex m_driveMutex;

	u32int m_base;	//I/O port base
	u8int m_irq;

	bool m_first;

	FloppyDrive* m_drives[2];

	void checkInterrupt(int *st0, int *cyl);
	void setDOR();			//Updates the Digital Output Register
	void setActiveDrive(u8int drive);	//Locks driveMutex and sets activeDrive.
	void setNoActiveDrive();			//Unlocks driveMutex
	bool writeCmd(u8int cmd);			//Sends command to controller
	u8int readData();					//Reads a byte from controller
	bool reset();
	
	public:
	static void detect();
	String getClass();
	String getName();

};

#endif

