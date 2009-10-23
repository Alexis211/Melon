#include <Sys.iface.h>
#include "Sys.ns.h"
#include <Syscall/RessourceCaller.class.h>

namespace Sys {

void halt() {
	RessourceCaller::sCall(SYIF_IFID, SYIF_HALT);
}

void reboot() {
	RessourceCaller::sCall(SYIF_IFID, SYIF_REBOOT);
}

}
