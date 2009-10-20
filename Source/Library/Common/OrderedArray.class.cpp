template <typename T>
OrderedArray<T>::OrderedArray(u32int max_size) {
	m_array = (T*)Memory::alloc(max_size * sizeof(T*));
	m_size = 0;
	m_maxSize = max_size;
}

template <typename T>
OrderedArray<T>::OrderedArray(T **addr, u32int max_size) {
	m_array = addr;
	memset((u8int*)addr, 0, max_size * sizeof(T*));
	m_size = 0;
	m_maxSize = max_size;
}

template <typename T>
OrderedArray<T>::~OrderedArray() {
	//Free memory
}

template <typename T>
void OrderedArray<T>::insert(T *element) {
	if (m_size == m_maxSize) return; //Array is full
	u32int iterator = 0;
	while (iterator < m_size && *(m_array[iterator]) < *element) {
		iterator++;
	}
	if (iterator == m_size) {
		m_array[m_size++] = element;
	} else {
		u32int pos = iterator;
		while (iterator < m_size) {
			iterator++;
			m_array[iterator] = m_array[iterator - 1];
		}
		m_size++;
		m_array[pos] = element;
	}
}

template <typename T>
T *OrderedArray<T>::lookup(int index) {
	return m_array[index];
}

template <typename T>
void OrderedArray<T>::remove(int index) {
	m_size--;
	while (index < m_size) {
		m_array[index] = m_array[index + 1];
		index++;
	}
}

