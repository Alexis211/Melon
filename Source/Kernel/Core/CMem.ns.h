#ifdef DEF_COMMON

#ifndef DEF_CMEM_NS_H
#define DEF_CMEM_NS_H

//This namespace contains basic memory managment functions

namespace CMem {
	u8int *memcpy(u8int *dest, const u8int *src, int count);
	u8int *memset(u8int *dest, u8int val, int count);
	u16int *memsetw(u16int *dest, u16int val, int count);
	u32int strlen(const char *str);
}

#endif

#endif
