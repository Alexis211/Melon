#include <Syscall/Syscall.wtf.h>
#include <WChar.class.h>
#include <Binding/VirtualTerminal.class.h>
#include <Binding/Thread.class.h>

int main() {
	VirtualTerminal x = VirtualTerminal::get();
	Thread t = Thread::get();
	for (char c = ' '; c <= 'z'; c++) {
		t.sleep((u32int)c / 4);
		x.put(c);
	}
	x.put("\n");
	x.writeHex(0xDEADBEEF);
	x.put("\n");
}
