#ifndef DEF_FATFS_CLASS_H
#define DEF_FATFS_CLASS_H

#include <VFS/FileSystem.proto.h>
#include <VFS/Part.ns.h>
#include <VFS/BlockCache.class.h>

struct fat_extBS_32_t {			//Extended boot sector for FAT32
	unsigned int		table_size_32;
	unsigned short		extended_flags;
	unsigned short		fat_version;
	unsigned int		root_cluster;
	unsigned short		fat_info;
	unsigned short		backup_BS_sector;
	unsigned char 		reserved_0[12];
	unsigned char		drive_number;
	unsigned char 		reserved_1;
	unsigned char		boot_signature;
	unsigned int 		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
}__attribute__((packed));

struct fat_extBS_16_t {	//Extended boot sector for FAT12 and FAT16
	unsigned char		bios_drive_num;
	unsigned char		reserved1;
	unsigned char		boot_signature;
	unsigned int		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
}__attribute__((packed));

struct fat_BS_t {		//Boot sector
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	        bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
	union {
		fat_extBS_32_t	extBS_32;
		fat_extBS_16_t	extBS_16;
	};
}__attribute__((packed));

struct fat_date_t {
	u16int year		: 7;
	u16int month	: 4;
	u16int day		: 5;
}__attribute__((packed));

struct fat_time_t {
	u16int hour		: 5;
	u16int minutes	: 6;
	u16int seconds	: 5;
}__attribute__((packed));

struct fat_dir_entry_t {
	char name[8];
	char extension[3];
	u8int attributes;
	u8int reserved;
	u8int createtenthsofseconds;
	fat_time_t created_time;
	fat_date_t created_date;
	fat_date_t accessed_date;
	u16int first_clust_high;	//always 0 on FAT12/16
	fat_time_t modified_time;
	fat_date_t modified_date;
	u16int first_clust_low;
	u32int size;
}__attribute__((packed));

#define FA_READONLY 0x01
#define FA_HIDDEN 0x02
#define FA_SYSTEM 0x04
#define FA_VOLUMEID 0x08
#define FA_DIRECTORY 0x10
#define FA_ARCHIVE 0x20
#define FA_LFN 0x0F

class FATFS : public FileSystem {
	private:
	fat_BS_t m_bs;
	bool m_readOnly;
	u32int m_fatSize;	//Size of one FAT, in sectors
	u32int m_totalSectors;	//Total sectors used by the FAT
	u32int m_rootDirSectors;	//Sectors used by the root directory
	u32int m_firstDataSector;	//First data sector, start of cluster 2
	u32int m_countOfClusters;	//Count of usable clusters, EXCLUDING the ones used by the root directory
	u32int m_clusterSize;	//size of a cluster in bytes
	u32int m_fatType;	//12, 16 or 32
	Partition* m_part;
	BlockCache<Partition> m_fatCache;

	u32int nextCluster(u32int cluster);		//Get the next cluster number in the chain (0 = EOF)
	bool readCluster(u32int cluster, u8int* data);	//Read the content of a cluster to a buffer
	
	~FATFS() {}
	FATFS(Partition* p) : m_part(p), m_fatCache(p) {}

	public:
	static FileSystem* mount(Partition* p, DirectoryNode* mountpoint, bool readwrite = false);

	bool unmount();

	String getDevDescription() { return Part::partIdentifier(m_part); }

	bool setName(FSNode* node, String name);
	bool setPermissions(FSNode* node, u32int permissions);
	bool setUid(FSNode* node, u32int uid);
	bool setGid(FSNode* node, u32int gid);
	bool setParent(FSNode* node, FSNode* parent);

	u32int read(FileNode* file, u64int position, u32int max_length, u8int *data);
	bool write(FileNode* file, u64int position, u32int length, u8int *data);
	bool truncate(FileNode* file);

	bool loadContents(DirectoryNode* dir);
	FileNode* createFile(DirectoryNode* parent, String name);
	DirectoryNode* createDirectory(DirectoryNode* parent, String name);
	bool remove(DirectoryNode* parent, FSNode* node);
};

#endif

