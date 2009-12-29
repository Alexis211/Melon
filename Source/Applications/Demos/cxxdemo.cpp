#include <Syscall/Syscall.wtf.h>
#include <Binding/Thread.class.h>
#include <Binding/File.class.h>

#include <App/ShellApp.proto.h>

class CPPDemo : public ShellApp {
	public:
	CPPDemo() : ShellApp("CPPDemo.app", "A demo application using the C++ Melon framework") {}
	int run();
};

APP(CPPDemo);

int CPPDemo::run() {
	outvt << "Enter some text plz : " << FLUSH;
	String s = invt.readLine();
	outvt << s;
	breakPoint();
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
