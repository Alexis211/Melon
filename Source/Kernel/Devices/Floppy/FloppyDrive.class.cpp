#include "FloppyDrive.class.h"
#include "FloppyController.class.h"
#include <TaskManager/Task.ns.h>
#include <DeviceManager/Time.ns.h>
#include <Core/Log.ns.h>

using namespace Sys;

FloppyDrive::FloppyDrive(FloppyController *fdc, u8int number, u8int type) {
	m_fdc = fdc;
	m_driveNumber = number;
	m_driveType = type;
	fdc->m_drives[number] = this;

	switch (m_driveType) {
		case FT_360K525:
			m_cylinders = 40;
			m_sectors = 9;
			break;
		case FT_12M525:
			m_cylinders = 80;
			m_sectors = 15;
			break;
		case FT_720K35:
			m_cylinders = 80;
			m_sectors = 9;
			break;
		case FT_288M35:
			m_cylinders = 80;
			m_sectors = 36;
			break;
		case FT_144M35:
		default:		//all unknown types are 1.44M
			m_cylinders = 80;
			m_sectors = 18;
			break;
	}
	m_buffCyl = 0xFFFF;	//Invalid cylinder
	m_buffTime = 0;
}

String FloppyDrive::getClass() {
	return "block.floppy";
}

String FloppyDrive::getName() {
	char *fdTypes[] = {
		"NONE",
		"360kB 5.25in",
		"1.2MB 5.25in",
		"720kB 3.5in",
		"1.44MB 3.5in",
		"2.88MB 3.5in" };
	String ret;
	if (m_driveNumber == 0) {
		ret = "Floppy drive #0 : ";
	} else {
		ret = "Floppy drive #1 : ";
	}
	ret += fdTypes[m_driveType];
	return ret;
}

bool FloppyDrive::setup() {
	m_fdc->setActiveDrive(m_driveNumber);

	//Set transfer rate
	u8int xfer_rate;
	if (m_driveType == FT_360K525) {
		xfer_rate = 2;
	} else if (m_driveType == FT_720K35) {
		xfer_rate = 1;
	} else {
		xfer_rate = 0;
	}
	outb(m_fdc->m_base + FR_CCR, xfer_rate);

	m_fdc->writeCmd(FC_SPECIFY);
	m_fdc->writeCmd(0xDF);
	m_fdc->writeCmd(0x02);

	bool ret = calibrate();

	m_fdc->setNoActiveDrive();

	return ret;
}

bool FloppyDrive::calibrate() {
	if (!m_fdc->m_driveMutex.locked() or m_fdc->m_activeDrive != m_driveNumber) return false;

	int st0, cyl = -1;

	if (!setMotorState(true)) return false;

	for (int i = 0; i < 10; i++) {
		asm volatile("cli");
		m_fdc->writeCmd(FC_RECALIBRATE);
		m_fdc->writeCmd(m_driveNumber);

		Task::currentThread->waitIRQ(m_fdc->m_irq);
		m_fdc->checkInterrupt(&st0, &cyl);
		asm volatile("sti");

		if (st0 & 0xC0) {
			continue;
		}
		if (!cyl) {	//Cylinder 0 reached
			setMotorState(false);
			return true;
		}
	}
	setMotorState(false);
	return false;
}

bool FloppyDrive::setMotorState(bool on) {
	if (!m_fdc->m_driveMutex.locked() or m_fdc->m_activeDrive != m_driveNumber) return false;

	if (on) {
		if (m_motorState == FS_MOTOROFF) {
			m_motorState = FS_MOTORON;
			m_fdc->setDOR();
			Task::currentThread->sleep(500);
		}
		m_motorState = FS_MOTORON;
	} else {
		m_motorState = FS_MOTORWAIT;
		m_motorTimeout = 4;
	}
	return true;
}

bool FloppyDrive::killMotor() {
	m_motorState = FS_MOTOROFF;
	m_fdc->setDOR();
	return true;
}

bool FloppyDrive::seek(u32int cyli, s32int head) {
	if (cyli >= m_cylinders) return false;
	m_fdc->setActiveDrive(m_driveNumber);

	setMotorState(true);	//Turn on motor

	int st0, cyl = -1;

	for (u32int i = 0; i < 10; i++) {
		asm volatile ("cli");
		m_fdc->writeCmd(FC_SEEK);
		m_fdc->writeCmd(head << 2);
		m_fdc->writeCmd(cyl);

		Task::currentThread->waitIRQ(m_fdc->m_irq);
		m_fdc->checkInterrupt(&st0, &cyl);
		asm volatile("sti");

		if (st0 & 0xC0) { //Error
			continue;
		}
		if (cyl == 0xFF or cyl == 0x00) {	//0xFF for bochs, 0x00 for qemu :D
			setMotorState(false);
			m_fdc->setNoActiveDrive();
			return true;
		}
		if (cyl == (int)cyli) {
			setMotorState(false);
			m_fdc->setNoActiveDrive();
			return true;
		}
	}
	setMotorState(false);
	m_fdc->setNoActiveDrive();
	return false;
}

bool FloppyDrive::doTrack(u32int cyl, u8int dir) {
	if (!seek(cyl, 0)) return false;
	if (!seek(cyl, 1)) return false;
	m_fdc->setActiveDrive(m_driveNumber);

	u8int cmd, flags = 0xC0;
	switch (dir) {
		case FD_READ:
			cmd = FC_READ_DATA | flags;
			break;
		case FD_WRITE:
			cmd = FC_WRITE_DATA | flags;
			break;
		default:
			m_fdc->setNoActiveDrive();
			return false;
	}

	for (int i = 0; i < 20; i++) {
		setMotorState(true);

		if (!FloppyController::dmaInit(dir, 0x4800)) {
			m_fdc->setNoActiveDrive();
			setMotorState(false);
			return false;
		}

		Task::currentThread->sleep(100);

		asm volatile("cli");
		m_fdc->writeCmd(cmd);
		m_fdc->writeCmd(m_driveNumber);	//Drive number & first head << 2
		m_fdc->writeCmd(cyl);	//Cylinder
		m_fdc->writeCmd(0);		//First head
		m_fdc->writeCmd(1);		//First sector
		m_fdc->writeCmd(2);
		m_fdc->writeCmd(18);	//Number of sectors
		m_fdc->writeCmd(0x1B);
		m_fdc->writeCmd(0xFF);

		Task::currentThread->waitIRQ(m_fdc->m_irq);

		u8int st0, st1, st2, rcy, rhe, rse, bps;
		st0 = m_fdc->readData();
		st1 = m_fdc->readData();
		st2 = m_fdc->readData();
		rcy = m_fdc->readData();
		rhe = m_fdc->readData();
		rse = m_fdc->readData();
		bps = m_fdc->readData();
		asm volatile("sti");

		int error = 0;
	
		if (st0 & 0xC0) error = 1;
		if (st1 & 0x80) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : end of cyilnder."); error = 1; }
		if (st0 & 0x08) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : drive not ready."); error = 1; }
		if (st1 & 0x20)	{ Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : CRC error."); error = 1; }
		if (st1 & 0x10) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : controller timeout."); error = 1; }
		if (st1 & 0x04) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : no data found."); error = 1; }
		if ((st2|st1) & 0x01) { 
			Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : no address mark found.");
			error=1;
		}
		if (st2 & 0x40) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : deleted address mark."); error = 1; }
		if (st2 & 0x20) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : CRC error in data."); error = 1; }
		if (st2 & 0x10) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : wrong cylinder."); error = 1; }
		if (st2 & 0x04) {
			Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : uPD765 sector not found.");
			error = 1;
		}
		if (st2 & 0x02) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : bad cylinder."); error = 1; }
		if (bps != 0x2) {
			Log::log(KL_ERROR, String("FloppyDrive.class : error while I/O : wanted 512 bytes/sector, got ")
					+= String::number(1 << (bps + 7)));
			error = 1;
		}
		if (st1 & 0x02) { Log::log(KL_ERROR, "FloppyDrive.class : error while I/O : not writable."); error = 2; }

		if (!error) CMem::memcpy(m_buffer, FloppyController::dmabuff, FLOPPY_DMALEN);	//Copy data to internal buffer
		FloppyController::dmaRelease();

		if (!error) {
			setMotorState(false);
			m_fdc->setNoActiveDrive();
			return true;
		}
		if (error > 1) {
			//Not retrying
			setMotorState(false);
			m_fdc->setNoActiveDrive();
			return false;
		}
	}
	setMotorState(false);
	return false;
}

bool FloppyDrive::readOnly() {
	m_fdc->setActiveDrive(m_driveNumber);
	asm volatile("cli");
	m_fdc->writeCmd(FC_SENSE_DRIVE_STATUS);
	m_fdc->writeCmd(m_driveNumber);
	Task::currentThread->waitIRQ(m_fdc->m_irq);
	u8int st3 = m_fdc->readData();
	asm volatile("sti");

	bool ret = (st3 & 0x80 ? true : false);

	m_fdc->setNoActiveDrive();

	return ret;
}

u64int FloppyDrive::blocks() {
	return m_cylinders * m_sectors * 2;
}

bool FloppyDrive::readBlocks(u64int start, u32int count, u8int *data) {
	u32int startblock = start;
	if (count == 1) {
		u32int cylinder = (startblock / (m_sectors * 2)), offset = (startblock % (m_sectors * 2)) * 512;
		if (m_buffCyl == cylinder && m_buffTime >= Time::uptime() - 4) {
			memcpy(data, (const u8int*)(&m_buffer[offset]), 512);
			return true;
		} else {
			if (!doTrack(cylinder, FD_READ)) return false;
			m_buffCyl = cylinder;
			m_buffTime = Time::uptime();
			memcpy(data, (const u8int*)(&m_buffer[offset]), 512);
			return true;
		}
	} else {
		m_buffCyl = 0xFFFF;	//Invalid cylinder
		m_buffTime = 0;

		u32int startcylinder = (startblock / (m_sectors * 2)), stoffset = startblock % (m_sectors * 2);
		u32int endcylinder = ((startblock + count - 1) / (m_sectors * 2)), 
			   endoffset = (startblock + count - 1) % (m_sectors * 2);
		for (u32int i = startcylinder; i <= endcylinder; i++) {
			if (!doTrack(i, FD_READ)) return false;	//Read cylinder
			//Figure out what to memcpy in data
			u32int start = 0;	//Where to start memcpying in buffer
			if (i == startcylinder) start = stoffset * 512;
			u32int count = FLOPPY_DMALEN - start;	//How much we must memcpy
			if (i == endcylinder) count = (endoffset * 512) - start + 512;
			u32int where = 0;
			if (i != startcylinder) where = (i - startcylinder - 1) * (512 * 2 * m_sectors) + (stoffset * 512);
			memcpy(&data[where], (const u8int*)(&m_buffer[start]), count);
		}
	}
	return true;
}

bool FloppyDrive::writeBlocks(u64int start, u32int count, u8int *data) { //TODO
	return false;
}

u32int FloppyDrive::blockSize() {
	return 512;
}

u64int FloppyDrive::chs2lba(u32int cylinder, u32int head, u32int sector) {
	return ((u64int)cylinder * (u64int)m_cylinders) + ((u64int)head + 2ULL) + (u64int)sector - 1ULL;
}
