#include <types.h>
#include <Syscall/Syscall.wtf.h>
#include <Binding/VirtualTerminal.class.h>

#include <Heap.class.h>

extern "C" void __cxa_pure_virtual() {}	//Required when using abstract classes
void *__dso_handle;		//Required when using global objects
extern "C" int __cxa_atexit(void (*f)(void*), void *p, void *d) { return 0; }

extern u32int start_ctors, end_ctors, start_dtors, end_dtors;

Heap heap;

VirtualTerminal invt(0), outvt(0);

int main(const Vector<String>& args);

void doExit(u32int v) {
	asm volatile("int $66" : : "a"(v));
}

extern "C" void start() {
	//Call static constructors
	u32int i = 0;
    for(u32int * call = &start_ctors; call < &end_ctors; call++) {
        ((void (*)(void))*call)();
    }

	heap.create(0x40000000, 0x00040000, 0x00004000);	//Initially create a 256ko heap with 16ko index
	invt = VirtualTerminal::getIn(); outvt = VirtualTerminal::getOut();
	if (!invt.valid()) doExit(1);
	if (!outvt.valid()) doExit(2);

	u32int argc = Process::get().argc();
	Vector<String> args(argc);
	for (u32int i = 0; i < argc; i++) args[i] = Process::get().argv(i);

	u32int r = main(args);

	//Call static destructors 
    for(u32int * call = &start_dtors; call < &end_dtors; call++) {
        ((void (*)(void))*call)();
    }

	doExit(r);
}

namespace Mem {
	void* alloc (size_t sz) { return heap.alloc(sz); }
	void free(void* ptr) { heap.free(ptr); }
	void* mkXchgSpace (size_t sz) { return alloc(sz); }
}
