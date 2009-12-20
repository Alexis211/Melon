#include "SB.ns.h"
#include <Mutex.class.h>
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>
#include <DeviceManager/Disp.ns.h>

namespace SB {

SimpleVT *sb = NULL;

String msg;

u16int boot_progress = 0;
bool gone_multi = false;

Mutex lock(MUTEX_FALSE);

void init() {
	sb = new SimpleVT(1, 80, STATUSBAR_FGCOLOR, STATUSBAR_BGCOLOR);
	sb->map(0, 0);
	sb->hideCursor();
}

void reinit() {
	if (!gone_multi) return;
	lock.waitLock();
	delete sb;
	sb = new SimpleVT(1, Disp::mode.textCols, STATUSBAR_FGCOLOR, STATUSBAR_BGCOLOR);
	sb->map(0, 0);
	sb->hideCursor();
	lock.unlock();
}

void drawprogress(const String& s) {
	*sb << "\nMelon is loading :       {";
	for (u32int i = 0; i < boot_progress; i++) *sb << ":";
	*sb << ".";
	sb->moveCursor(0, 45);
	*sb << "}";
	sb->moveCursor(0, 51);
	*sb << "[" << s;
	sb->moveCursor(0, 78);
	*sb << "]";
}

void drawnormal() {
	*sb << "\n [Up:" << (s64int)Time::uptime() << "s";
	sb->moveCursor(0, 10);
	*sb << "] [Mem:" << (s64int)PhysMem::free() << "/" << (s64int)PhysMem::total();
	sb->moveCursor(0, 28);
	*sb << "] [Proc.:" << (s64int)Task::processes->size();
	sb->moveCursor(0, 39);
	*sb << "] [Threads:" << (s64int)Task::threads->size();
	sb->moveCursor(0, 52);
	*sb << "] [" << msg;
	sb->moveCursor(0, sb->width() - 2);
	*sb << "]";
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
