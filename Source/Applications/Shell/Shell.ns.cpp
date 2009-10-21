#include "Shell.ns.h"

namespace Shell {

FSNode node(0);

u32int run() {
	node = FSNode::getRoot();
	while (1) {
		outvt << node.getName() << " : ";
		String s = invt.readLine();
	}
}

}
