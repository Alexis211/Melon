#include "KernelShell.class.h"
#include <VFS/VFS.ns.h>
#include <VFS/File.class.h>
#include <TaskManager/Task.ns.h>
#include <MemoryManager/PhysMem.ns.h>

void KernelShell::ls(Vector<String>& args) {
	DirectoryNode* d = m_cwd;
	if (args.size() == 2) {
		FSNode* n = VFS::find(args[1], m_cwd);
		d = NULL;
		if (n == NULL)
			*m_vt << "No such directory : " << args[1] << "\n";
		else if (n->type() != NT_DIRECTORY)
			*m_vt << "Not a directory : " << args[1] << "\n";
		else
			d = (DirectoryNode*)n;
	}	
	if (d != NULL) *m_vt << "Contents of directory " << VFS::path(d) << " :\n";
	for (u32int i = 0; d != NULL && i < d->getLength(); i++) {
		FSNode* n = d->getChild(i);
		if (n->type() == NT_FILE) {
			FileNode* f = (FileNode*)n;
			*m_vt << " - FILE\t" << f->getName();
			m_vt->setCursorCol(30);
			*m_vt << (s32int)f->getLength() << " bytes.\n";
		} else if (n->type() == NT_DIRECTORY) {
			*m_vt << " - DIR\t" << n->getName() << "/";
			m_vt->setCursorCol(30);
			*m_vt << (s32int)n->getLength() << " items.\n";
		}
	}
}

void KernelShell::cd(Vector<String>& args) {
	if (args.size() == 1) {
		*m_vt << "Error : no argument given.\n";
	} else {
		FSNode* n = VFS::find(args[1], m_cwd);
		if (n == NULL)
			*m_vt << "No such directory : " << args[1] << "\n";
		else if (n->type() != NT_DIRECTORY)
			*m_vt << "Not a directory : " << args[1] << "\n";
		else
			m_cwd = (DirectoryNode*)n;
	}
}

void KernelShell::pwd(Vector<String>& args) {
	*m_vt << "Current location : " << VFS::path(m_cwd) << "\n";
}

void KernelShell::cat(Vector<String>& args) {
	if (args.size() == 1) *m_vt << "Meow.\n";
	for (u32int i = 1; i < args.size(); i++) {
		File f(args[i], FM_READ, m_cwd);
		if (f.valid()) {
			u8int *buff = (u8int*)Mem::kalloc(f.length() + 1);
			f.read(f.length(), buff);
			buff[f.length()] = 0;
			*m_vt << String((const char*) buff);
			Mem::kfree(buff);
		} else {
			*m_vt << "Error reading from file " << args[i] << "\n";
		}
	}
}

void KernelShell::mkdir(Vector<String>& args) {
	if (args.size() > 1) {
		for (u32int i = 1; i < args.size(); i++) {
			if (VFS::createDirectory(args[i], m_cwd) == NULL)
				*m_vt << "Error while creating directory " << args[i] << "\n";
		}
	} else {
		*m_vt << "No argument specified.\n";
	}
}

void KernelShell::rm(Vector<String>& args) {
	if (args.size() == 1) *m_vt << "Error : no argument specified.\n";
	for (u32int i = 1; i < args.size(); i++) {
		if (!VFS::remove(args[i], m_cwd)) {
				*m_vt << "Error while removing file " << args[i] << "\n";
			}
	}
}

void KernelShell::wf(Vector<String>& args) {
	if (args.size() == 1) {
		*m_vt << "No file to write !\n";
	} else {
		File f(args[1], FM_TRUNCATE, m_cwd);
		if (f.valid()) {
			String t = m_vt->readLine();
			while (t != ".") {
				t += "\n";
				ByteArray temp(t);
				f.write(temp);
				t = m_vt->readLine();
			}
		} else {
			*m_vt << "Error openning file.\n";
		}
	}
}

void KernelShell::run(Vector<String>& args) {
	if (args.size() == 1) {
		*m_vt << "No app to run !\n";
	} else {
		File f(args[1], FM_READ, m_cwd);
		if (f.valid()) {
			u32int magic = 0;
			f.read<u32int>(&magic);
			if (magic == 0xFEEDBEEF) {
				u32int size;
				f.read<u32int>(&size);
				Process* p;
				p = new Process(args[1], 0);
				p->setVirtualTerminal(m_vt);
				u8int *ptr = (u8int*)p->heap().alloc(size);
				f.read(size, ptr);
				new Thread(p, (thread_entry_t)ptr, 0);
				kernelPageDirectory->switchTo();
				while (p->getState() != P_FINISHED) Task::currThread()->sleep(10);
				delete p;
			} else {
				*m_vt << "Bad magic number : " << (u32int)magic << "\n";
			}
		} else {
			*m_vt << "Unable to read from file.\n";
		}
	}
}
