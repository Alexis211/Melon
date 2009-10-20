#ifndef DEF_MELONBINARY_CLASS_H
#define DEF_MELONBINARY_CLASS_H

#include <Linker/Binary.proto.h>

class MelonBinary : public Binary {
	private:
	u32int m_size;
	u32int m_org;
	u8int* m_data;

	MelonBinary() {}

	public:
	virtual ~MelonBinary();
	static Binary* load(File& file);

	thread_entry_t toProcess(Process* p);
};

#endif
