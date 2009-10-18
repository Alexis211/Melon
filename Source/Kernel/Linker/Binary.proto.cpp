#include "Binary.proto.h"

#include <Linker/MelonBinary.class.h>

Binary* (*loaders[])(File& file) = {
	&MelonBinary::load,

0 };

Binary* Binary::load(File& file) {
	Binary* r = 0;
	u32int i = 0;
	while (loaders[i] != 0) {
		r = loaders[i](file);	//Call loader
		if (r != 0) break;
		i++;
	}
	return r;
}
