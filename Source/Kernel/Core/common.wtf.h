#ifndef DEF_COMMON
#define DEF_COMMON

#include <Config.h>

//This file is very important : it contains type definitions, macro definitions and new/delete implementations.

#define NULL 0

#include <Core/types.wtf.h>

#include <Core/CMem.ns.h>
#include <Core/Sys.ns.h>

#include <MemoryManager/Mem.ns.h>

//Standard implemenations of operator new/delete
inline void* operator new(u32int, void *p) { return p; }
inline void* operator new[](u32int, void *p) { return p; }
inline void operator delete(void*, void*) { }
inline void operator delete[](void*, void*) { }

inline void* operator new(u32int sz) { return Mem::kalloc(sz); }
inline void* operator new[](u32int sz) { return Mem::kalloc(sz); }
inline void operator delete(void *ptr) { Mem::kfree(ptr); }
inline void operator delete[](void *ptr) { Mem::kfree(ptr); }

#endif
