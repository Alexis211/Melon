#ifndef DEF_RES_NS_H
#define DEF_RES_NS_H

#include <SyscallManager/Ressource.class.h>

namespace Res {

extern Ressource** ressources;
extern u32int size;

u32int registerRes(Ressource* r);
void unregisterRes(u32int id);

template <typename T>
T* get(u32int id, u32int type) {
	if (id > size or ressources[id] == 0) return 0;
	if (ressources[id]->resType() != type) return 0;
	return (T*)ressources[id];
}

u32int call(u32int ressource, u8int wat, u32int a, u32int b, u32int c, u32int d, u32int e);

}

#endif
