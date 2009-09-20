#ifndef DEF_VT_NS_H
#define DEF_VT_NS_H

#include <Core/common.wtf.h>
#include <VTManager/SimpleVT.class.h>

namespace VT {
	//These should be called only from inside class VirtualTerminal
	void map(SimpleVT* vt);
	void unmap(SimpleVT* vt);
	void redrawScreen();
}

#endif
