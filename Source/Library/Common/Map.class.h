#ifndef DEF_MAP_CLASS_H
#define DEF_MAP_CLASS_H

template <typename K, typename V>
class Map {
	private:
	V m_null;
	struct item_t {
		item_t *prev, *next;
		K key;
		V value;
		item_t(const K& _key, const V& _value) : prev(0), next(0), key(_key), value(_value) {}
		~item_t() {
			if (prev != 0) delete prev;
			if (next != 0) delete next;
		}
	} *m_root;

	//Recursive functions for internal use
	item_t* find(const K& key, item_t* start) {
		if (start == 0) return 0;
		if (start->key == key) return start;
		if (start->key > key) return find(key, start->prev);
		if (start->key < key) return find(key, start->next);
		return 0;
	}
	item_t* insert(const K& key, const V& value, item_t* start) {
		if (start == 0) return new item_t(key, value);
		if (start->key == key) return 0;
		if (start->key > key) {
			start->prev = insert(key, value, start->prev);
			return start;
		}
		if (start->key < key) {
			start->next = insert(key, value, start->next);
			return start;
		}
		return 0;
	}

	public:
	Map();
	~Map();

	bool has(const K& key);
	void set(const K& key, const V& value);
	const V& get(const K& key);
	V& operator[] (const K& key);
};

#include "Map.class.cpp"

#endif
