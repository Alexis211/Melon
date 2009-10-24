#ifndef DEF_FILE_CLASS_H
#define DEF_FILE_CLASS_H

#include <VFS/FileNode.class.h>
#include <ByteArray.class.h>
#include <File.iface.h>
#include <SyscallManager/Ressource.class.h>
class Process;

class File : public Ressource {
	protected:
	FileNode* m_file;
	bool m_valid;		//Is a file opened and valid ?
	bool m_writable;	//Is file opened for write ?
	u64int m_position;
	Process* m_process;
	
	//Syscalls
	static call_t m_callTable[];
	u32int closeSC();
	u32int validSC();
	u32int readSC(u32int, u32int);
	u32int writeSC(u32int, u32int);
	u32int seekSC(u32int, u32int, u32int);
	u32int positionSC();
	u32int lengthSC();
	u32int eofSC();
	bool accessible();

	public:
	static u32int scall(u8int, u32int, u32int, u32int, u32int);

	File();
	File(String filename, u8int mode = FM_READ, FSNode* start = 0);
	virtual ~File();

	bool open(String filename, u8int mode = FM_READ, FSNode* start = 0, bool vrfyperm = false);
	void close(bool unregisterFD = true);	//unregisterFD = whether or not we must unregister the file descriptor from process
	bool valid() { return m_valid; }

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
};

#endif
