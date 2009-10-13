#include "Heap.class.h"

/*
 * Implementation of the functions for managing heap index
 */

void Heap::insertIntoIndex(heap_header_t *e) {
	//If index is full, return
	if ((m_index.size * sizeof(heap_header_t*) + (u32int)m_index.data) >= m_start) return;

	u32int iterator = 0;
	while (iterator < m_index.size && m_index.data[iterator]->size < e->size) {
		if (m_index.data[iterator] == e) return;
		iterator++;
	}
	if (iterator == m_index.size) {
		m_index.data[m_index.size++] = e;
	} else {
		u32int pos = iterator;
		iterator = m_index.size;
		while (iterator > pos) {
			m_index.data[iterator] = m_index.data[iterator - 1];
			iterator--;
		}
		m_index.size++;
		m_index.data[pos] = e;
	}
}

u32int Heap::findIndexEntry(heap_header_t *e) {
	for (u32int  i = 0; i < m_index.size; i++) {
		if (m_index.data[i] == e)
			return i;
	}
	return (u32int) - 1;
}

void Heap::removeFromIndex(u32int idx) {
	m_index.size--;
	while (idx < m_index.size) {
		m_index.data[idx] = m_index.data[idx + 1];
		idx++;
	}
}

void Heap::removeFromIndex(heap_header_t *e) {
	u32int i = findIndexEntry(e);
	if (i != (u32int) - 1) {
		removeFromIndex(i);
	}
}
