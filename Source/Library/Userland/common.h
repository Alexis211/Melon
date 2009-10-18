#ifndef DEF_COMMON
#define DEF_COMMON

#define NULL 0

#include <types.h>

#include <CMem.ns.h>

namespace Mem {
	void* alloc(u32int);
	void free(void*);
}

//Standard implemenations of operator new/delete
inline void* operator new(u32int, void *p) { return p; }
inline void* operator new[](u32int, void *p) { return p; }
inline void operator delete(void*, void*) { }
inline void operator delete[](void*, void*) { }

inline void* operator new(u32int sz) { return Mem::alloc(sz); }
inline void* operator new[](u32int sz) { return Mem::alloc(sz); }
inline void operator delete(void *ptr) { Mem::free(ptr); }
inline void operator delete[](void *ptr) { Mem::free(ptr); }

#endif
