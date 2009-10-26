#ifndef DEF_COMMON
#define DEF_COMMON

#define NULL 0

#include <types.h>

#include <CMem.ns.h>

namespace Mem {
	void* alloc(size_t);
	void free(void*);
	void* mkXchgSpace(size_t sz);
}

//Standard implemenations of operator new/delete
inline void* operator new(size_t, void *p) { return p; }
inline void* operator new[](size_t, void *p) { return p; }
inline void operator delete(void*, void*) { }
inline void operator delete[](void*, void*) { }

inline void* operator new(size_t sz) { return Mem::alloc(sz); }
inline void* operator new[](size_t sz) { return Mem::alloc(sz); }
inline void operator delete(void *ptr) { Mem::free(ptr); }
inline void operator delete[](void *ptr) { Mem::free(ptr); }

#endif
