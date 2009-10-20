#ifndef DEF_BINARY_PROTO_H
#define DEF_BINARY_PROTO_H

#include <VFS/File.class.h>
#include <TaskManager/Process.class.h>
#include <TaskManager/Thread.class.h>

class Binary {
	public:
	static Binary* load(File& file);
	virtual ~Binary() {}

	virtual thread_entry_t toProcess(Process* p) = 0;
};

#endif
