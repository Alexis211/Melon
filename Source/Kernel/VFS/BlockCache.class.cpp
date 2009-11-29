#include <DeviceManager/Time.ns.h>

template <typename T>
BlockCache<T>::BlockCache(T* dev) {
	m_dev = dev;
	m_count = 0;
}

template <typename T>
BlockCache<T>::~BlockCache() {
	sync();
	m_count = 0;
	delete m_cache;
	delete m_cacheInfo;
}

template <typename T>
void BlockCache<T>::sync() {
	for (u32int i = 0; i < m_count; i++) {
		if (m_cacheInfo[i].dirty) m_dev->writeBlocks(m_cacheInfo[i].id, 1, m_cache + (i * m_dev->blockSize()));
	}	
}

template <typename T>
void BlockCache<T>::init(u32int count) {
	m_count = count;
	m_cacheInfo = new cached_block_t[count];
	for (u32int i = 0; i < m_count; i++) {
		m_cacheInfo[i].id = 0;
		m_cacheInfo[i].lastuse = 0;
		m_cacheInfo[i].dirty = false;
	}
	m_cache = (u8int*)Mem::alloc(m_count * m_dev->blockSize());
}

template <typename T>
bool BlockCache<T>::setCache(u64int block, u8int* data, bool dirty) {
	u32int best = 0;
	for (u32int i = 0; i < m_count; i++) {
		if (m_cacheInfo[i].id == block) {
			best = i;
			break;
		}
		if (m_cacheInfo[i].lastuse < m_cacheInfo[best].lastuse) best = i;
	}
	if (best >= m_count) return false;
	if (m_cacheInfo[best].dirty && (m_cacheInfo[best].id != block or !dirty))
	   m_dev->writeBlocks(m_cacheInfo[best].id, 1, m_cache + (best * m_dev->blockSize()));
	m_cacheInfo[best].id = block;
	m_cacheInfo[best].lastuse = Time::uptime();
	m_cacheInfo[best].dirty = dirty;
	memcpy(m_cache + (best * m_dev->blockSize()), data, m_dev->blockSize());
	return true;
}

template <typename T>
bool BlockCache<T>::getCache(u64int block, u8int* data) {
	for (u32int i = 0; i < m_count; i++) {
		if (m_cacheInfo[i].id == block && m_cacheInfo[i].lastuse != 0) {
			m_cacheInfo[i].lastuse = Time::uptime();
			memcpy(data, m_cache + (i * m_dev->blockSize()), m_dev->blockSize());
			return true;
		}
	}
	return false;
}

template <typename T>
bool BlockCache<T>::readBlocks(u64int startblock, u32int count, u8int* data) {
	if (count == 1) {
		if (getCache(startblock, data)) return true;
		if (!m_dev->readBlocks(startblock, count, data)) return false;
		setCache(startblock, data);
		return true;
	} else {
		return m_dev->readBlocks(startblock, count, data);
	}
}

template <typename T>
bool BlockCache<T>::writeBlocks(u64int startblock, u32int count, u8int* data) {
	if (m_dev->readOnly()) return false;
	if (count == 1) {
		if (!setCache(startblock, data, true)) return m_dev->writeBlocks(startblock, count, data);
		return true;
	} else {
		return m_dev->writeBlocks(startblock, count, data);
	}
}
