#ifndef DEF_SHELLAPP_CLASS_H
#define DEF_SHELLAPP_CLASS_H

#include <App/Application.proto.h>
#include <Binding/VirtualTerminal.class.h>

class ShellApp : public Application {
	public:
	VirtualTerminal invt, outvt;
	
	ShellApp() : Application(), invt(VirtualTerminal::getIn()), outvt(VirtualTerminal::getOut()) {
		if (!invt.valid()) exit(1);
		if (!outvt.valid()) exit(2);
	}
};

#endif

