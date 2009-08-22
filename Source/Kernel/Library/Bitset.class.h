#ifndef DEF_BITSET_CLASS_H
#define DEF_BITSET_CLASS_H

#include <Core/common.wtf.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

class Bitset {
	private:
	u32int m_size;
	u32int *m_data;
	u32int m_usedCount;

	public:
	Bitset();
	Bitset(u32int size);
	Bitset(u32int size, u32int *ptr);
	~Bitset();

	void init(u32int size, u32int *ptr);

	void setBit(u32int number);
	void clearBit(u32int number);
	bool testBit(u32int number);
	u32int firstFreeBit();

	u32int usedBits();
};

#endif
