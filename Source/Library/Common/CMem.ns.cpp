#include <common.h>

namespace CMem {

//Standard C functions
u8int *memcpy(u8int *dest, const u8int *src, int count) {
	for (int i = 0; i < count; i++) {
		dest[i] = src[i];
	}
	return dest;
}

u8int *memset(u8int *dest, u8int val, int count) {
	for (int i = 0; i < count; i++) {
		dest[i] = val;
	}
	return dest;
}

u16int *memsetw(u16int *dest, u16int val, int count) {
	for (int i = 0; i < count; i++) {
		dest[i] = val;
	}
	return dest;
}

u32int strlen(const char *str) {
	u32int i = 0;
	while (str[i]) {
		i++;
	}
	return i;
}

}
