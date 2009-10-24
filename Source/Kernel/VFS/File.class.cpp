#include "File.class.h"
#include <VFS/VFS.ns.h>
#include <TaskManager/Task.ns.h>

call_t File::m_callTable[] = {
	CALL0(FLIF_CLOSE, &File::closeSC),
	CALL0(FLIF_VALID, &File::validSC),
	CALL2(FLIF_READ, &File::readSC),
	CALL2(FLIF_WRITE, &File::writeSC),
	CALL3(FLIF_SEEK, &File::seekSC),
	CALL0(FLIF_POSITION, &File::positionSC),
	CALL0(FLIF_LENGTH, &File::lengthSC),
	CALL0(FLIF_EOF, &File::eofSC),
	CALL0(0, 0)
};

File::File() : Ressource(FLIF_OBJTYPE, m_callTable), m_file(NULL), m_valid(false), m_writable(false), m_position(0) {
}

File::File(String filename, u8int mode, FSNode* start) :
	Ressource(FLIF_OBJTYPE, m_callTable), m_file(NULL), m_valid(false), m_writable(false), m_position(0) {
	open(filename, mode, start);
}

File::~File() {
	close();
}

bool File::open(String filename, u8int mode, FSNode* start, bool vrfyperm) {
	if (m_valid) return false;

	FSNode* node = VFS::find(filename, start);
	if (node == NULL){
	   	if (mode == FM_READ) return false;
		node = VFS::createFile(filename, start, vrfyperm);
		if (node == 0) return false;
	}
	if (node->type() != NT_FILE) return false;

	m_file = (FileNode*) node;

	m_writable = (mode != FM_READ);
	if (vrfyperm and m_writable and !m_file->writable()) return false;
	if (vrfyperm and !m_file->readable()) return false;
	if (!m_file->fsWritable() and m_writable) return false;

	if (mode == FM_READ or mode == FM_REPLACE) {
		m_position = 0;
	} else if (mode == FM_TRUNCATE) {
		if (!m_file->truncate()) return false;
		m_position = 0;
	} else if (mode == FM_APPEND) {
		m_position = m_file->getLength();
	}

	if (m_writable)
		m_file->m_readers++;
	else
		m_file->m_writers++;

	m_process = Task::currProcess();
	m_process->registerFileDescriptor(this);
	m_valid = true;
	return true;
}

u32int File::read(u32int max_length, u8int *data) {
	if (!m_valid) return 0;
	u32int l = m_file->read(m_position, max_length, data);
	m_position += l;
	return l;
}

bool File::write(u32int length, u8int *data) {
	if (!m_valid) return false;
	if (!m_writable) return false;
	if (m_file->write(m_position, length, data)) {
		m_position += length;
		return true;
	}
	return false;
}

u32int File::read(ByteArray &data) {
	if (!m_valid) {
		data.clear();
		return 0;
	}
	u32int l = m_file->read(m_position, data.size(), (u8int*)data);
	m_position += l;
	if (l != data.size()) data.resize(l);
	return l;	
}

bool File::write(ByteArray &data) {
	if (!m_valid) return false;
	if (!m_writable) return false;
	if (m_file->write(m_position, data.size(), (u8int*)data)) {
		m_position += data.size();
		return true;
	}
	return false;
}

bool File::seek(u64int count, u8int mode) {
	if (!m_valid) return false;
	if (mode == SM_FORWARD) {
		if (m_position + count <= m_file->getLength()) {
			m_position += count;
			return true;
		} else {
			return false;
		}
	} else if (mode == SM_BACKWARD) {
		if (count <= m_position) {
			m_position -= count;
			return true;
		} else {
			return false;
		}
	} else if (mode == SM_BEGINNING) {
		if (count <= m_file->getLength()) {
			m_position = count;
			return true;
		} else {
			return false;
		}
	} else if (mode == SM_END) {
		if (count <= m_file->getLength()) {
			m_position = m_file->getLength() - count;
			return true;
		} else {
			return false;
		}
	}
	return false;
}

bool File::eof() {
	return m_position == m_file->getLength();
}

void File::close(bool unregisterFD) {
	if (!m_valid) return;
	if (m_writable)
		m_file->m_writers--;
	else
		m_file->m_readers--;
	m_valid = false;
	m_file = NULL;
	m_position = 0;
	m_writable = false;
	if (unregisterFD) m_process->unregisterFileDescriptor(this);
}
