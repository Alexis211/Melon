#ifndef DEF_VECTOR_CLASS
#define DEF_VECTOR_CLASS

#include <common.h>

template <typename T>
class Vector {
	private:
	T *m_data;
	u32int m_size;

	public:
	Vector();
	Vector(u32int size);
	Vector(u32int size, const T& value);
	Vector(const Vector<T> &other);
	Vector<T>& operator= (const Vector<T> &other);
	~Vector();

	T& operator[] (u32int index) const;
	
	void push(const T& element);
	//void push(T& element);
	void pop();

	T& back() const;
	T& front() const;

	u32int size() const;
	bool empty() const;
	void clear();
};

#include "Vector.class.cpp"

#endif
