#include <common.h>

namespace CMem {

//Standard C functions
u8int *memcpy(u8int *dest, const u8int *src, u32int count) {
	u32int f = count % 4, n = count / 4;
	const u32int* s = (u32int*)src;
	u32int* d = (u32int*)dest;
	for (u32int i = 0; i < n; i++) {
		d[i] = s[i];
	}
	if (f != 0) {
		for (u32int i = count - f; i < count; i++) {
			dest[i] = src[i];
		}
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
