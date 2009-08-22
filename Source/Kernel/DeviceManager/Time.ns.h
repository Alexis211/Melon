#ifndef DEF_TIME_NS_H
#define DEF_TIME_NS_H

#include <Devices/Timer.class.h>

namespace Time {
	void setTimer(Timer* t);

	u32int uptime();
	u32int time();
}


#endif
