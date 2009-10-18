#include <types.h>

int main();

extern "C" void start() {
	u32int r = main();
	asm volatile("int $66" : : "a"(r));
}
