#include "Log.ns.h"
#include <FileSystems/RamFS/RamFS.class.h>
#include <VFS/VFS.ns.h>

namespace Log {

TextFile *logs[7] = {0, 0, 0, 0, 0, 0, 0};

void init(u8int loglevel) {
	//Create directories
	if (VFS::find("/System") == 0) VFS::createDirectory("/System");
	if (VFS::find("/System/Logs") == 0) VFS::createDirectory("/System/Logs");

	logs[KL_PANIC] = new TextFile("/System/Logs/Panic.log", FM_APPEND);
	if (!logs[KL_PANIC]->valid()) {	//FS maybee not read/write, mount a ramfs
		RamFS::mount(1024*1024, (DirectoryNode*)VFS::find("/System/Logs"));
		logs[KL_PANIC] = new TextFile("/System/Logs/Panic.log", FM_APPEND);
	}
	if (KL_CRITICAL <= loglevel) logs[KL_CRITICAL] = new TextFile("/System/Logs/Critical.log", FM_APPEND);
	if (KL_ERROR <= loglevel) logs[KL_ERROR] = new TextFile("/System/Logs/Error.log", FM_APPEND);
	if (KL_WARNING <= loglevel) logs[KL_WARNING] = new TextFile("/System/Logs/Warning.log", FM_APPEND);
	if (KL_NOTICE <= loglevel) logs[KL_NOTICE] = new TextFile("/System/Logs/Notice.log", FM_APPEND);
	if (KL_STATUS <= loglevel) logs[KL_STATUS] = new TextFile("/System/Logs/Status.log", FM_APPEND);
	if (KL_DEBUG <= loglevel) logs[KL_DEBUG] = new TextFile("/System/Logs/Debug.log", FM_APPEND);
}

void close() {
	for (u32int i = 0; i < 7; i++) {
		if (logs[i] != 0) {
			delete logs[i];
			logs[i] = 0;
		}
	}
}

void log(u8int level, String event) {
	if (logs[level] != 0 and logs[level]->valid()) logs[level]->write(event, true);
}

}
