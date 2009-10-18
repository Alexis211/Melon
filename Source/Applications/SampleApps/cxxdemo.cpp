#include <Syscall/Syscall.wtf.h>
#include <WChar.class.h>
#include <VirtualTerminal.class.h>

int main() {
	VirtualTerminal x = VirtualTerminal::get();
	for (char c = ' '; c <= 'z'; c++) {
		sleep((u32int)c / 4);
		x.put(c);
	}
	x.put("\n");
	x.writeHex(0xDEADBEEF);
	x.put("\n");
}
