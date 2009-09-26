#ifndef DEF_FILE_CLASS_H
#define DEF_FILE_CLASS_H

#include <VFS/FileNode.class.h>
#include <Library/ByteArray.class.h>

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
	protected:
	FileNode* m_file;
	bool m_valid;		//Is a file opened and valid ?
	bool m_writable;	//Is file opened for write ?
	u64int m_position;

	public:
	File();
	File(String filename, u8int mode = FM_READ, FSNode* start = 0);
	virtual ~File();

	bool open(String filename, u8int mode = FM_READ, FSNode* start = 0);
	void close(bool unregisterFD = true);	//unregisterFD = whether or not we must unregister the file descriptor from process

	u32int read(u32int max_length, u8int *data);
	bool write(u32int length, u8int *data);
	u32int read(ByteArray &data);	//Fills ByteArray at its current length or shrinks it if we can't read enough
	bool write(ByteArray &data);

	template <typename T> bool read(T* elem) {
		return (read(sizeof(T), (u8int*)elem) == sizeof(T));
	}
	template <typename T> bool write(T* elem) {
		return write(sizeof(T), (u8int*)elem);
	}

	bool seek(u64int count, u8int mode);
	u64int position() { return m_position; }
	u64int length() { return m_file->getLength(); }
	bool eof();

	bool valid() { return m_valid; }
};

#endif
