#include "VT.ns.h"
#include <Library/Vector.class.h>
#include <DeviceManager/Disp.ns.h>

namespace VT {

Vector<VirtualTerminal*> mappedVTs;

void map(VirtualTerminal* vt) {
	unmap(vt);	//Bad things might happen
	mappedVTs.push(vt);
}

void unmap(VirtualTerminal* vt) {
	for (u32int i = 0; i < mappedVTs.size(); i++) {
		if (mappedVTs[i] == vt) {
			mappedVTs[i] = mappedVTs.back();
			mappedVTs.pop();
			redrawScreen();
			return;
		}
	}
}

void redrawScreen() {
	Disp::clear();
	for (u32int i = 0; i < mappedVTs.size(); i++) {
		mappedVTs[i]->redraw();
	}
}

}
