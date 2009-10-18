#include <Syscall/Syscall.wtf.h>
#include <WChar.class.h>
#include <Binding/VirtualTerminal.class.h>
#include <Binding/Thread.class.h>

int main() {
	VirtualTerminal x = VirtualTerminal::get();
	String s = x.readLine();
	x.write(s);
	Thread t = Thread::get();
	for (char c = ' '; c <= 'z'; c++) {
		t.sleep((u32int)c / 4);
		x.put(c);
	}
	x.put("\n");
	x.write("Salut les gens ! c'est le progrès !!!\nLe boeuf mort est juste là : ");
	x.writeHex(0xDEADBEEF);
	x.put("\n");
}
