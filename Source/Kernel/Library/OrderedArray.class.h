#ifndef DEF_ORDARRAY_CLASS
#define DEF_ORDARRAY_CLASS

#include <Core/common.wtf.h>

template <typename T>
class OrderedArray {
	private:
	T *m_array[];
	u32int m_size;
	u32int m_maxSize;

	public:
	OrderedArray(u32int max_size);
	OrderedArray(T **addr, u32int max_size);
	~OrderedArray();

	u32int size() { return m_size; }

	void insert(T *element);
	T *lookup(int index);
	void remove(int index);

	T *operator[] (int index) { return lookup(index); }
};

#include "OrderedArray.class.cpp"

#endif
