#include <Binding/VirtualTerminal.class.h>
#include <Binding/FSNode.class.h>
#include <String.class.h>

int main() {
	VirtualTerminal vt = VirtualTerminal::get();
	FSNode node = FSNode::getRoot();
	while (1) {
		vt << node.getName() << " : ";
		String s = vt.readLine();
	}
	return 0;
}
