#ifndef DEF_LOG_NS_H
#define DEF_LOG_NS_H

#include <TextFile.class.h>

#define KL_PANIC 0
#define KL_CRITICAL 1
#define KL_ERROR 2
#define KL_WARNING 3
#define KL_NOTICE 4
#define KL_STATUS 5
#define KL_DEBUG 6

namespace Log {
	void init(u8int loglevel);
	void close();
	void log(u8int level, String event);
}

#endif
