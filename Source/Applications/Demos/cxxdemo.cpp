#include <Syscall/Syscall.wtf.h>
#include <String.class.h>
#include <Binding/VirtualTerminal.class.h>
#include <Binding/Thread.class.h>
#include <Binding/File.class.h>

int main(const Vector<String>& args) {
	outvt << "Enter some text plz : ";
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
