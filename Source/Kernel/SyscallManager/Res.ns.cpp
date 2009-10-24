#include "Res.ns.h"

#include <VirtualTerminal.iface.h>
#include <Process.iface.h>
#include <Thread.iface.h>
#include <FSNode.iface.h>
#include <Sys.iface.h>
#include <TaskManager/Task.ns.h>
#include <VFS/File.class.h>

namespace Res {

typedef u32int (*staticcall)(u8int, u32int, u32int, u32int, u32int);

struct static_call_t {
	u32int id;
	staticcall call;
};

static_call_t staticCalls[] = {
	{VTIF_OBJTYPE, VirtualTerminal::scall},
	{PRIF_OBJTYPE, Process::scall},
	{THIF_OBJTYPE, Thread::scall},
	{FNIF_OBJTYPE, FSNode::scall},
	{FLIF_OBJTYPE, File::scall},
	{SYIF_IFID, Sys::scall},
	{0, 0}
};

Ressource** ressources = 0;
u32int size = 0;

void expand() {	//Expands size of ressources array of 20 entries
	size += 20;
	Ressource** tmp = (Ressource**)Mem::alloc(size * sizeof(Ressource*));
	for (u32int i = 0; i < size; i++) {
		if (i < size - 20) tmp[i] = ressources[i];
		else tmp[i] = 0;
	}
	Mem::free(ressources);
	ressources = tmp;
}

u32int registerRes(Ressource* r) {
	if (ressources == 0 or size == 0) {
		ressources = (Ressource**)Mem::alloc(20 * sizeof(Ressource*)); 
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
		for (u32int i = 0; staticCalls[i].id != 0; i++) {
			if (staticCalls[i].id == a) return staticCalls[i].call(wat, b, c, d, e);
		}
		return 0;
	} else {
		if (ressource > size or ressources[ressource] == 0) {
			return (u32int) - 1;
		} else {
			return ressources[ressource]->call(wat, a, b, c, d, e);
		}
	}
}

}
