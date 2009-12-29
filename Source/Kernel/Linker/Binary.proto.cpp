#include "Binary.proto.h"

#include <Linker/ElfBinary.class.h>

Binary* (*loaders[])(File& file) = {
	&ElfBinary::load,
0 };

Binary* Binary::load(File& file) {
	Binary* r = 0;
	u32int i = 0;
	while (loaders[i] != 0) {
		file.seek(0, SM_BEGINNING);
		r = loaders[i](file);	//Call loader
		if (r != 0) break;
		i++;
	}
	return r;
}
