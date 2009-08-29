#ifndef DEF_COMMON
#define DEF_COMMON

#include <Config.h>

//This file is very important : it contains type definitions, macro definitions and new/delete implementations.

#define NULL 0

typedef unsigned int		addr_t;
typedef unsigned long long	u64int;
typedef unsigned int		u32int;
typedef unsigned short		u16int;
typedef unsigned char		u8int;
typedef long long	s64int;
typedef int 		s32int;
typedef short		s16int;
typedef char		s8int;

#include <Core/CMem.ns.h>
#include <Core/Sys.ns.h>

#define PANIC(msg) Sys::panic(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : Sys::panic_assert(__FILE__, __LINE__, #b))

#ifdef OPT_DEBUG
#define DEBUG(m) Sys::bochs_output(m, __FILE__, __LINE__)
#define DEBUG_HEX(m) Sys::bochs_output_hex(m);
#else
#define DEBUG(m)
#define DEBUG_HEX(m)
#endif

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
