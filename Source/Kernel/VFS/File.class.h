#ifndef DEF_FILE_CLASS_H
#define DEF_FILE_CLASS_H

#include <VFS/FileNode.class.h>

enum {
	FM_READ = 0,		//Open for read, put cursor at beginning
	FM_TRUNCATE = 1,	//Open for write, truncating file before
	FM_APPEND = 2,		//Open for write, put cursor at end
	FM_REPLACE = 3		//Open for write, put cursor at beginning
};

enum {
	SM_FORWARD = 0,		//Seek from actual position
	SM_BACKWARD = 1,	//Seek from actual position, backward
	SM_BEGINNING = 2,	//Seek from start of file
	SM_END = 3,			//Seek from end of file
};

class File {
	private:
	FileNode* m_file;
	bool m_valid;		//Is a file opened and valid ?
	bool m_writable;	//Is file opened for write ?
	u64int m_position;

	public:
	File();
	File(String filename, u8int mode = FM_READ, FSNode* start = 0);
	~File();

	bool open(String filename, u8int mode = FM_READ, FSNode* start = 0);
	u32int read(u32int max_length, u8int *data);
	bool write(u32int length, u8int *data);
	bool seek(u64int count, u8int mode);
	u64int getPosition() { return m_position; }
	void close(bool unregisterFD = true);	//unregisterFD = whether or not we must unregister the file descriptor from process

	bool valid() { return m_valid; }
};

#endif
