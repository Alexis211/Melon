#include <types.h>
#include <Syscall/Syscall.wtf.h>

#include <App/Application.proto.h>

#include <Heap.class.h>

extern u32int start_ctors, end_ctors;

Heap *heap;

int main(const Vector<String>& args);

extern "C" void start() {
	Heap h;
	h.create(0x40000000, 0x00040000, 0x00004000);	//Initially create a 256ko heap with 16ko index
	heap = &h;

	//Call static constructors (this will construct the Application object and get some stuff (arguments, ...))
	u32int i = 0;
    for(u32int * call = &start_ctors; call < &end_ctors; call++) {
        ((void (*)(void))*call)();
    }

	app->init();
	app->doEvents();
	u32int r = app->run();
	app->doEvents();
	app->exit(r);	//Will call static destructors
}

namespace Mem {
	void* alloc (size_t sz) { return heap->alloc(sz); }
	void free(void* ptr) { heap->free(ptr); }
	void* mkXchgSpace (size_t sz) { return alloc(sz); }
}
