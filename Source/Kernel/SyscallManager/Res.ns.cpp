#include "Res.ns.h"

#include <VirtualTerminal.iface.h>
#include <Process.iface.h>
#include <Thread.iface.h>
#include <TaskManager/Task.ns.h>

namespace Res {

Ressource** ressources = 0;
u32int size = 0;

void expand() {	//Expands size of ressources array of 20 entries
	size += 20;
	Ressource** tmp = (Ressource**)Mem::kalloc(size * sizeof(Ressource*));
	for (u32int i = 0; i < size; i++) {
		if (i < size - 20) tmp[i] = ressources[i];
		else tmp[i] = 0;
	}
	Mem::kfree(ressources);
	ressources = tmp;
}

u32int registerRes(Ressource* r) {
	if (ressources == 0 or size == 0) {
		ressources = (Ressource**)Mem::kalloc(20 * sizeof(Ressource*)); 
		size = 20;
		for (u32int i = 0; i < 20; i++) ressources[i] = 0;
	}
	for (u32int i = 0; i < size; i++) {
		if (ressources[i] == 0) {
			ressources[i] = r;
			return i;
		}
	}
	expand();
	return registerRes(r);
}

void unregisterRes(u32int id) {
	ressources[id] = 0;
}

u32int call(u32int ressource, u8int wat, u32int a, u32int b, u32int c, u32int d, u32int e) {
	if (ressource == 0xFFFFFE) {	//TODO : return ressource id for some stuff for current process
		if (a == VT_IFACE_OBJTYPE) return Task::currProcess()->getVirtualTerminal()->resId();
		if (a == PR_IFACE_OBJTYPE) return Task::currProcess()->resId();
		if (a == TH_IFACE_OBJTYPE) return Task::currThread()->resId();
		return 0;
	} else {
		if (ressource > size or ressources[ressource] == 0) {
			return (u32int) - 1;
		} else {
			return ressources[ressource]->doCall(wat, a, b, c, d, e);
		}
	}
}

}
