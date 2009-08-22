#ifndef DEF_VECTOR_CLASS
#define DEF_VECTOR_CLASS

#include <Core/common.wtf.h>

template <typename T>
class Vector {
	private:
	T *m_data;
	u32int m_size;

	public:
	Vector();
	Vector(u32int size);
	Vector(u32int size, T value);
	Vector(const Vector<T> &other);
	~Vector();

	T& operator[] (u32int index);
	
	void push(T element);
	//void push(T& element);
	void pop();

	T& back();
	T& front();

	u32int size();
	bool empty();
	void clear();
};

#include "Vector.class.cpp"

#endif
