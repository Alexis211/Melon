#include "Time.ns.h"

namespace Time {

Timer* timer;

void setTimer(Timer* t) {
	timer = t;
}

u32int uptime() {
	return timer->uptime();
}

u32int time() {
	return timer->time();
}

}
