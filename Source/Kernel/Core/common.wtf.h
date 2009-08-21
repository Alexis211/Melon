#ifndef DEF_COMMON
#define DEF_COMMON

#include <Config.h>

#define NULL 0

typedef unsigned int	u32int;
typedef unsigned short	u16int;
typedef unsigned char	u8int;
typedef int 	s32int;
typedef short	s16int;
typedef char	s8int;

#include <Core/CMem.ns.h>
#include <Core/Sys.ns.h>

#define PANIC(msg) Sys::panic(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : Sys::panic_assert(__FILE__, __LINE__, #b))

#ifdef OPT_DEBUG
#define DEBUG(m) Sys::bochs_output(m, __FILE__, __LINE__)
#else
#define DEBUG(m)
#endif

#endif
