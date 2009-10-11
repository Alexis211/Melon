//This file just contains a few methods required for some C++ things to work
#include <Core/types.wtf.h>

extern "C" void __cxa_pure_virtual() {}	//Required when using abstract classes

void *__dso_handle;		//Required when using global objects
extern "C" int __cxa_atexit(void (*f)(void*), void *p, void *d) { return 0; }

//Functions for quad divisions/modulo. Taken and arranged from klibc include/asm/div64.h
//These only work with 32-bit divisors and only return 32-bit remainder.
//TODO : think of some correct quad div/mod algorithms
inline u64int doDiv(u64int dividend, u32int divisor, u32int *remainder) {
	union {
		u64int v64;
		u32int v32[2];
	} d = { dividend };
	u32int upper;

	upper = d.v32[1];
	d.v32[1] = 0;
	if (upper >= divisor) {
		d.v32[1] = upper / divisor;
		upper %= divisor;
	}
	asm ("divl %2" : "=a" (d.v32[0]), "=d" (*remainder) :
			"rm" (divisor), "0" (d.v32[0]), "1" (upper));
	return d.v64;
}

extern "C" {
u64int __udivdi3(u64int dividend, u64int b) {
	u32int divisor, remainder;
	divisor = b;
	return doDiv(dividend, divisor, &remainder);
}

u64int __umoddi3(u64int dividend, u64int b) {
	u32int divisor, remainder;
	divisor = b;
	doDiv(dividend, divisor, &remainder);
	return remainder;
}
}
