#include "FATFS.class.h"
#include <VTManager/SimpleVT.class.h>
#include "FATFileNode.class.h"
#include "FATDirectoryNode.class.h"
#include <VFS/VFS.ns.h>
#include <ByteArray.class.h>

#define FIRSTCLUS(node) ((u32int&)(node->type() == NT_DIRECTORY ? \
			((FATDirectoryNode*)(node))->m_firstCluster : \
			((FATFileNode*)(node))->m_firstCluster))
#define FIRSTDEID(node) ((u32int&)(node->type() == NT_DIRECTORY ? \
			((FATDirectoryNode*)(node))->m_firstDirEntryID : \
			((FATFileNode*)(node))->m_firstDirEntryID))

FileSystem* FATFS::mount(Partition* p, DirectoryNode* mountpoint, bool readwrite) {
	if (readwrite) return 0;
	if (mountpoint != 0 and !mountpoint->mountpointable()) return 0;
	//		***	READ BOOT SECTOR ***
	union {
		fat_BS_t s;
		u8int c[512];
	} bs;
	if (!p->readBlocks(0, 1, bs.c)) return 0;
	//		*** CHECK FILESYSTEM TYPE ***
	if (bs.s.extBS_16.boot_signature != 0x28 and bs.s.extBS_16.boot_signature != 0x29
			and bs.s.extBS_32.boot_signature != 0x28 and bs.s.extBS_32.boot_signature != 0x29) return 0;
	//		*** DO SOME CALCULATIONS ***
	FATFS* fs = new FATFS(p);
	fs->m_fatSize = (bs.s.table_size_16 == 0 ? bs.s.extBS_32.table_size_32 : bs.s.table_size_16);
	fs->m_totalSectors = (bs.s.total_sectors_16 == 0 ? bs.s.total_sectors_32 : bs.s.total_sectors_16);
	fs->m_rootDirSectors = ((bs.s.root_entry_count * 32) + (bs.s.bytes_per_sector - 1)) / bs.s.bytes_per_sector;
	fs->m_firstDataSector = bs.s.reserved_sector_count + (fs->m_fatSize * bs.s.table_count);
	fs->m_clusterSize = bs.s.bytes_per_sector * bs.s.sectors_per_cluster;
	u32int dataSectors = fs->m_totalSectors - (fs->m_firstDataSector + fs->m_rootDirSectors);
	fs->m_countOfClusters = dataSectors / bs.s.sectors_per_cluster;
	if (fs->m_countOfClusters < 4085) {	//Find out FAT type
		fs->m_fatType = 12;
	} else if (fs->m_countOfClusters < 65525) {
		fs->m_fatType = 16;
	} else {
		fs->m_fatType = 32;
	}
	fs->m_readOnly = true;
	fs->m_bs = bs.s;
	//		*** CREATE ROOT DIRECTORY NODE ***
	fs->m_rootNode = new FATDirectoryNode("/", fs, mountpoint);
	FIRSTCLUS(fs->m_rootNode) = 2;
	if (fs->m_fatType == 32) FIRSTCLUS(fs->m_rootNode) = bs.s.extBS_32.root_cluster;
	if (!fs->m_rootNode->loadContent()) {
		*kvt << "Could not read FAT filesystem root directory.\n";
		delete fs;
		return 0;
	}
	fs->m_fatCache.init(fs->m_fatType == 12 ? 8 : (fs->m_fatType == 16 ? 20 : 40));
	if (mountpoint != 0) mountpoint->mount(fs->m_rootNode);
	VFS::registerFilesystem(fs);
	*kvt << "Detected a FAT" << (s64int)fs->m_fatType << " filesystem.\n" <<
		"root_dir_sectors:" << fs->m_rootDirSectors << " fat_size:" << fs->m_fatSize << " total_sectors:" <<
		fs->m_totalSectors << " data_sectors:" << dataSectors << " count_of_clusters:" << fs->m_countOfClusters <<
		" sizeof(fat_dir_entry_t):" << sizeof(fat_dir_entry_t) << " first_data_sector:" << fs->m_firstDataSector <<
		" cluster_size:" << fs->m_clusterSize << "\n";
	return fs;
}

u32int FATFS::nextCluster(u32int cluster) {
	u8int fat_table[m_part->blockSize()];
	u32int val;
	if (m_fatType == 12) {
		u32int fat_offset = cluster + (cluster / 2);
		u32int fat_sector = m_bs.reserved_sector_count + (fat_offset / m_part->blockSize());
		u32int ent_offset = fat_offset % m_part->blockSize();
		m_fatCache.readBlocks(fat_sector, 1, fat_table);
		u16int tblval = *((u16int*)(fat_table + ent_offset));
		if (cluster & 1) val = tblval >> 4;
		else val = tblval & 0x0FFF;
		if (val >= 0xFF7) val = 0;
	} else if (m_fatType == 16) {
		u32int fat_offset = cluster * 2;
		u32int fat_sector = m_bs.reserved_sector_count + (fat_offset / m_part->blockSize());
		u32int ent_offset = fat_offset % m_part->blockSize();
		m_fatCache.readBlocks(fat_sector, 1, fat_table);
		u16int tblval = *(u16int*)(fat_table + ent_offset);
		val = tblval;
		if (tblval >= 0xFFF7) val = 0;
	} else if (m_fatType == 32) {
		u32int fat_offset = cluster * 4;
		u32int fat_sector = m_bs.reserved_sector_count + (fat_offset / m_part->blockSize());
		u32int ent_offset = fat_offset % m_part->blockSize();
		m_fatCache.readBlocks(fat_sector, 1, fat_table);
		val = *(u32int*)(fat_table + ent_offset) & 0x0FFFFFFF;
		if (val >= 0x0FFFFFF7) val = 0;
	} else {
		val = 0;
	}
	return val;
}

bool FATFS::readCluster(u32int cluster, u8int* data) {
	u32int firstSector = ((cluster - 2) * m_bs.sectors_per_cluster) + m_firstDataSector;
	if (cluster > 2 and m_fatType != 32) firstSector += m_rootDirSectors;
	return m_part->readBlocks(firstSector, m_bs.sectors_per_cluster, data);
}

bool FATFS::unmount() {
	if (m_readOnly) return true;
	return false;
}

bool FATFS::setName(FSNode* node, String name) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::setPermissions(FSNode* node, u32int permissions) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::setUid(FSNode* node, u32int uid) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::setGid(FSNode* node, u32int gid) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::setParent(FSNode* node, FSNode* parent) {
	if (m_readOnly) return false;
	return false;
}

u32int FATFS::read(FileNode* file, u64int position, u32int max_length, u8int *data) {
	u32int len = max_length;
	if (position >= file->getLength()) return 0;
	if (position + len > file->getLength()) len = file->getLength() - position;
	u32int firstCluster = position / m_clusterSize, clusterOffset = position % m_clusterSize;
	u32int clusters = (len + clusterOffset) / m_clusterSize + 1, lastClusBytesToRead = (len + clusterOffset) % m_clusterSize;
	u32int clust = FIRSTCLUS(file);
	//Find first cluster
	for (u32int i = 0; i < firstCluster and clust != 0; i++) clust = nextCluster(clust);
	if (clust == 0) return 0;
	//Read first cluster
	u8int* temp = (u8int*)Mem::alloc(m_clusterSize);
	readCluster(clust, temp);
	memcpy(data, temp + clusterOffset, (len > m_clusterSize - clusterOffset ? m_clusterSize - clusterOffset : len));
	//Read next cluster
	u32int pos = (m_clusterSize - clusterOffset);
	for (u32int i = 1; i < clusters; i++) {
		clust = nextCluster(clust);
		if (clust == 0) break;
		readCluster(clust, temp);
		memcpy(data + pos, temp, (i == clusters - 1 ? lastClusBytesToRead : m_clusterSize));
		pos += m_clusterSize;
	}
	Mem::free(temp);
	return len;
}

bool FATFS::write(FileNode* file, u64int position, u32int length, u8int* data) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::truncate(FileNode* file) {
	if (m_readOnly) return false;
	return false;
}

bool FATFS::loadContents(DirectoryNode* dir) {
	u32int cluster = FIRSTCLUS(dir);
	union {
		u8int *c;
		fat_dir_entry_t *e;
	} e;

	u32int entries = m_clusterSize / sizeof(fat_dir_entry_t);
	if (cluster == 2 and m_fatType != 32) {		//This is the value we use for the root directory
		e.c = (u8int*)Mem::alloc(m_rootDirSectors * m_part->blockSize());
		if (!m_part->readBlocks(m_firstDataSector, m_rootDirSectors, e.c)) return false;
	} else {
		e.c = (u8int*)Mem::alloc(m_clusterSize);
	}

	ByteArray lfnBuffer;
	while (cluster != 0) {
		if (cluster != 2 or m_fatType == 32) {
			if (!readCluster(cluster, e.c)) return false;
		}
		for (u32int i = 0; i < entries; i++) {
			if (e.e[i].attributes == FA_LFN && e.c[i*32] != 0xE5) {	//Long file name entry
				u8int num = e.c[i*32] & 0x3;
				if (lfnBuffer.size() < num * 26) lfnBuffer.resize(num * 26);
				num--;
				memcpy(lfnBuffer + (num * 26), e.c + (i*32 + 1), 10);
				memcpy(lfnBuffer + (num * 26 + 10), e.c + (i*32 + 14), 12);
				memcpy(lfnBuffer + (num * 26 + 22), e.c + (i*32 + 28), 4);
			}
			if (e.e[i].attributes & FA_VOLUMEID) continue;
			if (e.e[i].name[0] == 0  or e.e[i].name[0] == 0xE5) continue;	//Nothing intresting here.
			String name;
			if (lfnBuffer.empty()) {
				for (int j = 0; j < 8; j++) {
					if (e.e[i].name[j] == ' ') break;
					name += WChar(e.e[i].name[j]);
				}
				for (int j = 0; j < 3; j++) {
					if (e.e[i].extension[j] == ' ') break;
					if (j == 0) name += ".";
					name += WChar(e.e[i].extension[j]);
				}
			} else {
				name = lfnBuffer.toString(UE_UTF16_LE);
				lfnBuffer.clear();
			}
			u32int first_clus = (e.e[i].first_clust_high << 16) + e.e[i].first_clust_low;
			FSNode* n;
			if (e.e[i].attributes & FA_DIRECTORY) {
				if (name == "." or name == "..") continue;
				n = new FATDirectoryNode(name, this, dir);
			} else {
				n = new FATFileNode(name, this, dir, e.e[i].size);
			}
			FIRSTCLUS(n) = first_clus;
			dir->getChildren().push(n);
		}
		if (cluster == 2 && m_fatType != 32) break;	//We are in a FAT12/16 root directory
		cluster = nextCluster(cluster);
	}

	Mem::free(e.c);
	return true;
}

FileNode* FATFS::createFile(DirectoryNode* parent, String name) {
	if (m_readOnly) return false;
	return 0;
}

DirectoryNode* FATFS::createDirectory(DirectoryNode* parent, String name) {
	if (m_readOnly) return false;
	return 0;
}

bool FATFS::remove(DirectoryNode* parent, FSNode* node) {
	if (m_readOnly) return false;
	return false;
}
