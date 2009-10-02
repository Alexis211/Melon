#include "FloppyController.class.h"
#include "FloppyDrive.class.h"
#include <TaskManager/Task.ns.h>
#include <DeviceManager/Dev.ns.h>
#include <VFS/Part.ns.h>

using namespace Sys;	//For outb/inb

//***********************************************************
//				STATIC, FOR DMA
//***********************************************************
Mutex FloppyController::dmaMutex(false);
u8int FloppyController::dmabuff[FLOPPY_DMALEN]
	__attribute__((aligned(0x8000)));

bool FloppyController::dmaInit(u8int direction, u32int length) {
	dmaMutex.waitLock();

	union {
		u8int b[4];
		u32int l;
	} a, c;			//Address, count

	//We want the physical address of dmabuff. We simply remove 0xC0000000, because we've mapped memory linearly.
	a.l = (u32int) &dmabuff - 0xC0000000;
	c.l = (u32int) length - 1;

	if (
			(a.l >> 24) ||		//Address must be under 16mb
			(c.l >> 16) ||		//Count must be < 64k
			(((a.l & 0xFFFF) + c.l) >> 16)	//We must not cross a 64k boundary
	   ) {			//Something is wrong
		dmaMutex.unlock();
		return false;
	}

	u8int mode;
	switch (direction) {
		case FD_READ:
			mode = 0x46;
			break;
		case FD_WRITE:
			mode = 0x4A;
			break;
		default:		//Invalid direction
			dmaMutex.unlock();
			return false;
	}

	outb(0x0a, 0x06);	//Mask chan 2

	outb(0x0c, 0xff);	//Reset flip-flop
	outb(0x04, a.b[0]);	//Address low
	outb(0x04, a.b[1]);	//Address high

	outb(0x81, a.b[2]);	//External page register

	outb(0x0c, 0xff);	//Reset flip-flop
	outb(0x05, c.b[0]);	//Count low
	outb(0x05, c.b[1]);	//Count high

	outb(0x0b, mode);	//Mode

	outb(0x0a, 0x02);	//Unmask chan 2

	return true;
}

void FloppyController::dmaRelease() {
	dmaMutex.unlock();
}

//*********************************************************
//				FOR THE CONTROLLER
//*********************************************************
u32int floppyMotorTimer(void* plop) {	//This will be an independant thread
	while(1) {
		Task::currentThread->sleep(1000);	//Check only every second
		Vector<Device*> floppys = Dev::findDevices("block.floppy");
		for (u32int i = 0; i < floppys.size(); i++) {
			FloppyDrive* f = (FloppyDrive*)floppys[i];
			if (f->m_motorTimeout > 0 && f->m_motorState == FS_MOTORWAIT) {
				f->m_motorTimeout--;
				if (f->m_motorTimeout == 0)
					f->killMotor();
			}
		}
	}
	return 0;
}

FloppyController::FloppyController(u32int base, u8int irq) : m_driveMutex(false) {
	m_activeDrive = 0;
	m_base = base;
	m_irq = irq;
	m_drives[0] = NULL;
	m_drives[1] = NULL;
	m_first = false;
}

void FloppyController::detect() {	//TODO : do this better
	FloppyController *fdc = new FloppyController(0x03F0, 6);	//Standard controller, IRQ6 and base port 0x03F0
	Dev::registerDevice(fdc);

	outb(0x70, 0x10);	//CMOS detect
	u8int drives = inb(0x71);

	u8int fdd0 = (drives >> 4), fdd1 = (drives & 0x0F);
	
	if (fdd0 != FT_NONE)
		Dev::registerDevice(new FloppyDrive(fdc, 0, fdd0));
	if (fdd1 != FT_NONE)
		Dev::registerDevice(new FloppyDrive(fdc, 1, fdd1));
	fdc->reset();

	Vector<Device*> fdds = Dev::findDevices("block.floppy");
	for (u32int i = 0; i < fdds.size(); i++) {
		Part::registerDevice((BlockDevice*)fdds[i]);
	}

	new Thread(floppyMotorTimer, 0, true);
}

String FloppyController::getClass() {
	return "controller.floppy";
}

String FloppyController::getName() {
	String irq = String::number(m_irq);
	return String("Floppy controller at IRQ ") += irq;
}

void FloppyController::checkInterrupt(int *st0, int *cyl) {
	writeCmd(FC_SENSE_INTERRUPT);
	*st0 = readData();
	*cyl = readData();
}

void FloppyController::setDOR() {
	u8int dor = 0x0C;
	if (m_activeDrive == 1)
		dor |= 0x01;
	if (m_drives[0] != NULL and m_drives[0]->m_motorState != 0)
		dor |= 0x10;
	if (m_drives[1] != NULL and m_drives[1]->m_motorState != 0)
		dor |= 0x20;
	asm volatile ("cli");
	outb(m_base + FR_DOR, dor);
	if (m_first) {	//First time we set the DOR, controller initialized
		Task::currentThread->waitIRQ(m_irq);
		int st0, cyl;
		checkInterrupt(&st0, &cyl);
		m_first = false;
	}
	asm volatile ("sti");
			//PANIC("test");
}

void FloppyController::setActiveDrive(u8int drive) {
	m_driveMutex.waitLock();
	m_activeDrive = drive;
	setDOR();
}

void FloppyController::setNoActiveDrive() {
	m_driveMutex.unlock();
}

bool FloppyController::writeCmd(u8int cmd) {
	for (int i = 0; i < 600; i++) {
		if (0x80 & inb(m_base + FR_MSR)) {
			outb(m_base + FR_FIFO, cmd);
			return true;
		}
		Task::currentThread->sleep(10);
	}
	return false;
}

u8int FloppyController::readData() {
	for (int i = 0; i < 600; i++) {
		if (0x80 & inb(m_base + FR_MSR)) {
			return inb(m_base + FR_FIFO);
		}
		Task::currentThread->sleep(10);
	}
	return 0;
}

bool FloppyController::reset() {
	outb(m_base + FR_DOR, 0x00);	//Disable controller
	m_first = true;
	setNoActiveDrive();

	if (m_drives[0] != NULL) m_drives[0]->m_motorState = 0;
	if (m_drives[1] != NULL) m_drives[1]->m_motorState = 0;

	for (int i = 0; i < 2; i++) {
		if (m_drives[i] != NULL) {
			if (!m_drives[i]->setup())
				return false;
		}
	}
	return true;
}
