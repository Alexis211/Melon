#ifndef DEF_VT_NS_H
#define DEF_VT_NS_H

#include <Core/common.wtf.h>
#include <VTManager/VirtualTerminal.class.h>

namespace VT {
	//These should be called only from inside class VirtualTerminal
	void map(VirtualTerminal* vt);
	void unmap(VirtualTerminal* vt);
	void redrawScreen();
}

#endif
