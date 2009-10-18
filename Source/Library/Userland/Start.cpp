#include <types.h>

#include <Heap.class.h>

extern "C" void __cxa_pure_virtual() {}	//Required when using abstract classes
void *__dso_handle;		//Required when using global objects
extern "C" int __cxa_atexit(void (*f)(void*), void *p, void *d) { return 0; }

extern u32int start_ctors, end_ctors, start_dtors, end_dtors;

Heap heap;

int main();

extern "C" void start() {
	//Call static constructors
    for(u32int * call = &start_ctors; call < &end_ctors; call++) {
        ((void (*)(void))*call)();
    }

	heap.create(0x40000000, 0x00100000, 0x00004000);	//Initially create a 1M heap with 16ko index
	u32int r = main();

	//Call static destructors 
    for(u32int * call = &start_dtors; call < &end_dtors; call++) {
        ((void (*)(void))*call)();
    }

	asm volatile("int $66" : : "a"(r));
}

namespace Mem {
	void* alloc (u32int sz) { return heap.alloc(sz); }
	void free(void* ptr) { heap.free(ptr); }
	void* mkXchgSpace (u32int sz) { return alloc(sz); }
}
