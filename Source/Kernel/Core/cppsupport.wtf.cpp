//This file just contains a few methods required for some C++ things to work

extern "C" void __cxa_pure_virtual() {}	//Required when using abstract classes

void *__dso_handle;		//Required when using global objects
extern "C" int __cxa_atexit(void (*f)(void*), void *p, void *d) { return 0; }
