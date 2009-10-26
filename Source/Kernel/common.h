#ifndef DEF_COMMON
#define DEF_COMMON

#include <Config.h>

//This file is very important : it contains type definitions, macro definitions and new/delete implementations.

#define NULL 0

#include <types.h>

#include <CMem.ns.h>
#include <Core/Sys.ns.h>

#include <MemoryManager/Mem.ns.h>

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
