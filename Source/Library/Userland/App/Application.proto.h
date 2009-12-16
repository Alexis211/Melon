#ifndef DEF_APPLICATION_PROTO_H
#define DEF_APPLICATION_PROTO_H

#include <Binding/Process.class.h>
#include <String.class.h>

extern u32int start_dtors, end_dtors;

#define APP(t) static t the_app; \
   	Application *app = &the_app;

class Application {
	public:
	Process pr;
	Vector<String> args;

	Application() : pr(Process::get()), args(pr.argc()) {
		for (u32int i = 0; i < args.size(); i++) args[i] = pr.argv(i);
		//TODO : add default signal handlers
	}
	virtual ~Application() {}

	virtual int run() = 0; //Application's main loop

	virtual void doEvents() {
		//TODO : handle signals (IPC featurs to come)
	}

	void exit(u32int ret) {
		//Call static destructors 
		for(u32int * call = &start_dtors; call < &end_dtors; call++) {
			((void (*)(void))*call)();
		}
		threadFinishedSyscall(ret);
	}
};

extern Application *app;

#endif

