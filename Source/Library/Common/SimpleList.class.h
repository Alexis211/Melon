#ifndef DEF_SIMPLELIST_CLASS_H
#define DEF_SIMPLELIST_CLASS_H

#include <common.h>

/* This class implements a singly linked list. It is also used to represent one of its elements. */

template <typename T>
class SimpleList {
	protected:
	T m_value;
	SimpleList<T>* m_next;

	public:
	SimpleList(const T& value, SimpleList<T>* next = 0) : m_value(value), m_next(next) {}
	~SimpleList() {
		if (m_next != 0)
			delete m_next;
	}

 	T& v() { return m_value; }
	T& operator* () { return m_value; }

	SimpleList<T>* cons(const T& value) {
		return new SimpleList<T>(value, this);
	}

	SimpleList<T>* next() {
		return m_next;
	}

	SimpleList<T>* last() {
		if (m_next == 0) return this;
		return m_next->last();
	}

	SimpleList<T>* delThis() {
		SimpleList<T>* ret = m_next;
		Mem::kfree(this);
		return ret;
	}

	void delNext() {
		if (m_next == 0) return;
		SimpleList<T>* temp = m_next;
		m_next = m_next->m_next;
		Mem::kfree(temp);
	}

	SimpleList<T>* removeOnce(const T& value) {
		if (value == m_value) return delThis();
		for (SimpleList<T> *iter = this; iter->next() != 0; iter = iter->next()) {
			if (iter->next()->v() == value) {
				iter->delNext();
				break;
			}
		}
		return this;
	}

	bool isEnd() {
		return m_next == 0;
	}

	u32int size() {
		if (m_next == 0)
			return 0;
		return m_next->size() + 1;
	}
};

#endif
