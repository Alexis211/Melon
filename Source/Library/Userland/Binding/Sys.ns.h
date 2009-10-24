#ifndef DEF_SYS_NS_H
#define DEF_SYS_NS_H

#include <Sys.iface.h>
#include <Syscall/RessourceCaller.class.h>

namespace Sys {
	
inline void halt() {
	RessourceCaller::sCall(SYIF_IFID, SYIF_HALT);
}

inline void reboot() {
	RessourceCaller::sCall(SYIF_IFID, SYIF_REBOOT);
}

inline u32int uptime() {	//Returns uptime in seconds
	return RessourceCaller::sCall(SYIF_IFID, SYIF_UPTIME);
}

inline u32int totalRam() { 	//Returns total amount of RAM in Ko
	return RessourceCaller::sCall(SYIF_IFID, SYIF_TOTALRAM);
}

inline u32int freeRam() { 	//Returns free amount of RAM in Ko
	return RessourceCaller::sCall(SYIF_IFID, SYIF_FREERAM);
}

}

#endif
