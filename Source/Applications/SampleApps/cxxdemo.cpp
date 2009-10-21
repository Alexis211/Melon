#include <Syscall/Syscall.wtf.h>
#include <WChar.class.h>
#include <Binding/VirtualTerminal.class.h>
#include <Binding/Thread.class.h>

int main() {
	String s = invt.readLine();
	outvt << s;
	Thread t = Thread::get();
	for (char c = ' '; c <= 'z'; c++) {
		t.sleep((u32int)c / 4);
		outvt.put(c);
	}
	outvt << "\n";
	outvt << "Salut les gens ! c'est le progrès !!!\nLe boeuf mort est juste là : ";
	outvt << 0xDEADBEEF;
	outvt << "\n";
}
