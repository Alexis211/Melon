#include "SB.ns.h"
#include <Mutex.class.h>
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <DeviceManager/Disp.ns.h>

#define BOOT_ITEMS 17

namespace SB {

SimpleVT *sb = NULL;

String msg;

u16int boot_progress = 0;
u16int height;
bool gone_multi = false;

Mutex lock(MUTEX_FALSE);

void init() {
	sb = new SimpleVT(1, 80, STATUSBAR_FGCOLOR, STATUSBAR_BGCOLOR);
	height = 1;
	sb->map(0, 0);
	sb->hideCursor();
}

void drawprogress(const String& s) {
	sb->clear();
	int d = (sb->width() / 2) - ((BOOT_ITEMS + 4) / 2);
	if (height == 1) d -= 4;
	sb->moveCursor(0, d);
	*sb << "{";
	for (u32int i = 0; i < boot_progress; i++) *sb << ":";
	*sb << ".";
	sb->moveCursor(0, BOOT_ITEMS + d + 3);
	*sb << "}";
	if (height > 1) {
		*sb << "\n";
		*sb << "Loading : [" << s;
		sb->moveCursor(1, sb->width() - 2);
		*sb << "]";
	} else {
		sb->moveCursor(0, 0);
		*sb << "Melon is loading :";
		sb->moveCursor(0, d + BOOT_ITEMS + 6);
		*sb << "[" << s;
		sb->moveCursor(0, sb->width() - 2);
		*sb << "]";
	}
}

void drawnormal() {
	sb->clear();
	sb->moveCursor(0, 0);
	*sb << " [Up:" << (s64int)Time::uptime() << "s";
	sb->moveCursor(0, 10);
	*sb << "] [Mem:" << (s64int)PhysMem::free() << "/" << (s64int)PhysMem::total();
	sb->moveCursor(0, 28);
	*sb << "] [Proc.:" << (s64int)Task::processes->size();
	sb->setCursorCol(39);
	if (height > 1) {
		*sb << "]";
		sb->moveCursor(1, 0);
		*sb << " [Threads:" << (s64int)Task::threads->size();
		sb->setCursorCol(12);
		*sb << "] [" << msg;
		sb->setCursorCol(sb->width() - 2);
		*sb << "]";
	} else {
		*sb << "] [Threads:" << (s64int)Task::threads->size();
		sb->setCursorCol(52);
		*sb << "] [" << msg;
		sb->setCursorCol(sb->width() - 2);
		*sb << "]";
	}
}

void reinit() {
	if (!gone_multi) return;
	lock.waitLock();
	delete sb;
	height = (Disp::mode.textCols < 80 ? 2 : 1);
	sb = new SimpleVT(height, Disp::mode.textCols, STATUSBAR_FGCOLOR, STATUSBAR_BGCOLOR);
	sb->map(0, 0);
	sb->hideCursor();
	if (boot_progress != 0) {
		drawprogress(msg);
	} else {
		drawnormal();
	}
	lock.unlock();
}

u32int thread(void*) {
	while (!gone_multi);	//Synchronize
	while (1) {
		lock.waitLock();
		if (boot_progress != 0) {
			drawprogress(msg);
		} else {
			drawnormal();
		}
		lock.unlock();
		Task::currThread()->sleep(100);
	}
	return 0;
}

void gomulti() {
	new Thread(thread, 0, true);
	gone_multi = true;
}

void progress(const String& s) {
	if (gone_multi) {
		lock.waitLock();
		boot_progress++;
		msg = s;
		lock.unlock();
	} else {
		boot_progress++;
		drawprogress(s);
	}
}

void message(const String& s) {
	if (!gone_multi) return;
	lock.waitLock();
	boot_progress = 0;
	msg = s;
	drawnormal();
	lock.unlock();
}

} //END NS
