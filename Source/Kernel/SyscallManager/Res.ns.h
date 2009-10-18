#ifndef DEF_RES_NS_H
#define DEF_RES_NS_H

#include <SyscallManager/Ressource.class.h>

namespace Res {

u32int registerRes(Ressource* r);
void unregisterRes(u32int id);

u32int call(u32int ressource, u8int wat, u32int a, u32int b, u32int c, u32int d, u32int e);

}

#endif
