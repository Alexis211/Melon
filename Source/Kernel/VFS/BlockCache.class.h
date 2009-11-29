#ifndef DEF_BLOCKCACHE_CLASS_H
#define DEF_BLOCKCACHE_CLASS_H

template <typename T>
class BlockCache {
	private:
	T* m_dev;
	u32int m_count;
	struct cached_block_t {
		u64int id;
		u32int lastuse;
		bool dirty;
	} *m_cacheInfo;
	u8int* m_cache;

	void sync();
	bool setCache(u64int block, u8int* data, bool dirty = false);
	bool getCache(u64int block, u8int* data);

	public:
	BlockCache(T* dev);
	~BlockCache();
	void init(u32int count);

	bool readBlocks(u64int startblock, u32int count, u8int* data);
	bool writeBlocks(u64int startblock, u32int count, u8int* data);
	
};

#include "BlockCache.class.cpp"

#endif
