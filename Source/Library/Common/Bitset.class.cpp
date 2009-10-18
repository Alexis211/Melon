#include "Bitset.class.h"

Bitset::Bitset() {
}

Bitset::Bitset(u32int size) {
	init(size, (u32int*)Mem::alloc(INDEX_FROM_BIT(size)));
}

Bitset::Bitset(u32int size, u32int *ptr) {
	init(size, ptr);
}

Bitset::~Bitset() {
	Mem::free(m_data);
}

void Bitset::init(u32int size, u32int *ptr) {
	m_size = size;
	m_data = ptr;
	for (u32int i = 0; i < INDEX_FROM_BIT(m_size); i++) {
		m_data[i] = 0;
	}
	m_usedCount = 0;
}

void Bitset::setBit(u32int number) {
	u32int idx = INDEX_FROM_BIT(number);
	u32int off = OFFSET_FROM_BIT(number);
	m_data[idx] |= (0x1 << off);
	m_usedCount++;
}

void Bitset::clearBit(u32int number) {
	u32int idx = INDEX_FROM_BIT(number);
	u32int off = OFFSET_FROM_BIT(number);
	m_data[idx] &= ~(0x1 << off);
	m_usedCount--;
}

bool Bitset::testBit(u32int number) {
	u32int idx = INDEX_FROM_BIT(number);
	u32int off = OFFSET_FROM_BIT(number);
	return (m_data[idx] & (0x1 << off));
}

u32int Bitset::firstFreeBit() {
	for (u32int i = 0; i < INDEX_FROM_BIT(m_size); i++) {
		if (m_data[i] != 0xFFFFFFFF) {
			for (int j = 0; j < 32; j++) {
				if (!(m_data[i] & (0x1 << j))) {
					return (i * 4 * 8) + j;
				}
			}
		}
	}
	return (u32int) - 1;
}

u32int Bitset::usedBits() {
	return m_usedCount;
}
