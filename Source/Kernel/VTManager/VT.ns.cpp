#include "VT.ns.h"
#include <Library/Vector.class.h>
#include <DeviceManager/Disp.ns.h>

namespace VT {

Vector<SimpleVT*> mappedVTs;

void map(SimpleVT* vt) {
	unmap(vt);	//Bad things might happen
	mappedVTs.push(vt);
}

void unmap(SimpleVT* vt) {
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
