template <typename K, typename V>
Map<K, V>::Map() {
	m_root = 0;
}

template <typename K, typename V>
Map<K, V>::~Map() {
	if (m_root != 0) delete m_root;
}

template <typename K, typename V>
bool Map<K, V>::has(const K& key) {
	return (find(key, m_root) != 0);
}

template <typename K, typename V>
void Map<K, V>::set(const K& key, const V& value) {
	item_t* i = find(key, m_root);
	if (i == 0) m_root = insert(key, value, m_root);
	else i->value = value;
}

template <typename K, typename V>
const V& Map<K, V>::get(const K& key) {
	item_t* i = find(key);
	if (i == 0) return m_null;
	return i->value;
}

template <typename K, typename V>
V& Map<K, V>::operator[] (const K& key) {
	item_t* i = find(key, m_root);
	if (i == 0) {
		m_root = insert(key, m_null, m_root);
		i = find(key, m_root);
	}
	return i->value;
}
