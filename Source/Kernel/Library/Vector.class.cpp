using namespace CMem;	//strlen and memcpy

template <typename T>
Vector<T>::Vector() {
	//DEBUG_HEX((u32int)this); DEBUG(" NEW EMPTY");
	//DEBUG_HEX(sizeof(T)); DEBUG("  sizeof(T)");
	m_data = 0;
	m_size = 0;
}

template <typename T>
Vector<T>::Vector(u32int size) {
	//DEBUG_HEX((u32int)this); DEBUG(" NEW ZERO");
	m_data = new T[size];
	m_size = size;
}

template <typename T>
Vector<T>::Vector(u32int size, T value) {
	//DEBUG_HEX((u32int)this); DEBUG(" NEW FILLED");
	//m_data = (T*)Mem::kalloc(size * sizeof(T));
	m_data = new T[size](value);
	m_size = size;
	/*for (u32int i = 0; i < size; i++) {
		m_data[i] = new(&m_data[i]) T(value);
	}*/
}

template <typename T>
Vector<T>::Vector(const Vector<T> &other) {
	//DEBUG_HEX((u32int)this); DEBUG(" COPY REF");
	m_size = other.m_size;
	m_data = (T*)Mem::kalloc(m_size * sizeof(T));
	for (u32int i = 0; i < m_size; i++) {
		new(&m_data[i]) T(other.m_data[i]);
	}
}

template <typename T>
Vector<T>& Vector<T>::operator= (const Vector<T> &other) {
	//DEBUG_HEX((u32int)this); DEBUG(" COPY EQ");
	if (m_data != 0) delete[] m_data;
	m_size = other.m_size;
	m_data = (T*)Mem::kalloc(m_size * sizeof(T));
	for (u32int i = 0; i < m_size; i++) {
		new(&m_data[i]) T(other.m_data[i]);
	}
}

template <typename T>
Vector<T>::~Vector() {
	//DEBUG_HEX((u32int)this); DEBUG(" DELETE");
	if (m_data != 0) delete[] m_data;
}

template <typename T>
T& Vector<T>::operator[] (u32int index) {
	return m_data[index];
}

template <typename T>
void Vector<T>::push(T element) {
	T* newdata = (T*)Mem::kalloc((m_size + 1) * sizeof(T));
	memcpy((u8int*)newdata, (const u8int*) m_data, m_size * sizeof(T));
	new(&newdata[m_size]) T(element);			//Construct by copy
	m_size++;
	Mem::kfree(m_data);
	m_data = newdata;
}

/*  template <typename T>
void Vector<T>::push(T& element) {
	T* newdata = (T*)Memory::alloc((m_size + 1) * sizeof(T));
	memcpy((u8int*)newdata, (const u8int*) m_data, m_size * sizeof(T));
	//memcpy((u8int*)newdata + (m_size * sizeof(T)), (const u8int*) element, sizeof(T));	//Copy
	new(&newdata[m_size]) T(element);			//Construct by copy
	m_size++;
	Memory::free(m_data);
	m_data = newdata;
} */

template <typename T>
void Vector<T>::pop() {
	m_size--;
	//delete(&m_data[m_size], &m_data[m_size]);		//implicitly call destructor with placement delete
	m_data[m_size].~T();	//Call destructor
	T* newdata = (T*)Mem::kalloc(m_size * sizeof(T));
	memcpy((u8int*)newdata, (const u8int*) m_data, m_size * sizeof(T));
	Mem::kfree(m_data);
	m_data = newdata;
}

template <typename T>
T& Vector<T>::back() {
	return m_data[m_size - 1];
}

template <typename T>
T& Vector<T>::front() {
	return m_data[0];
}


template <typename T>
u32int Vector<T>::size() {
	return m_size;
}

template <typename T>
bool Vector<T>::empty() {
	return m_size == 0;
}

template <typename T>
void Vector<T>::clear() {
	if (empty()) return;
	delete [] m_data;
	m_data = 0;
	m_size = 0;
}
