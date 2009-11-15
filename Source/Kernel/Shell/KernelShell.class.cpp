#include "KernelShell.class.h"
#include <VTManager/ScrollableVT.class.h>
#include <DeviceManager/Kbd.ns.h>
#include <SimpleList.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <VFS/VFS.ns.h>
#include <FileSystems/RamFS/RamFS.class.h>
#include <TaskManager/Task.ns.h>

u32int KernelShell::m_instances = 0;

u32int shellRun(void* ks) {
	KernelShell* sh = (KernelShell*)ks;
	u32int ret = sh->run();
	delete sh;
	return ret;
}

void KernelShell::setup(DirectoryNode* cwd, VirtualTerminal *vt) {
	m_vt = vt;
	Task::currProcess()->setInVT(vt);
	Task::currProcess()->setOutVT(vt);
	m_cwd = cwd;
	*m_vt << "Welcome to Melon's kernel shell !\n";
	m_thread = new Thread(shellRun, (void*)this, true);
	m_instances++;
}

KernelShell::KernelShell(DirectoryNode* cwd, VirtualTerminal* vt) {
	setup(cwd, vt);
}

KernelShell::KernelShell(DirectoryNode* cwd) {
	ScrollableVT* vt = new ScrollableVT(15, 76, 200, KVT_FGCOLOR, KVT_BGCOLOR);
	vt->map(9);
	Kbd::setFocus(vt);
	setup(cwd, vt);
}

KernelShell::~KernelShell() {
	delete m_vt;
	m_instances--;
}

u32int KernelShell::run() {
	struct {
		const char* name;
		void (KernelShell::*cmd)(Vector<String>&);
	} commands[]= {
		{"ls",    &KernelShell::ls},
		{"cd",    &KernelShell::cd},
		{"pwd",   &KernelShell::pwd},
		{"cat",   &KernelShell::cat},
		{"mkdir", &KernelShell::mkdir},
		{"rm",    &KernelShell::rm},
		{"wf",    &KernelShell::wf},
		{"run",	  &KernelShell::run},

		{"devices", &KernelShell::devices},
		{"loadkeys", &KernelShell::loadkeys},
		{"free", &KernelShell::free},
		{"uptime", &KernelShell::uptime},
		{"part", &KernelShell::part},

		{0, 0}
	};
	
	while (1) {
		m_vt->setColor(KVT_LIGHTCOLOR);
		*m_vt << VFS::path(m_cwd) << " # ";
		m_vt->setColor(KVT_ENTRYCOLOR);
		Vector<String> tokens = m_vt->readLine().split(" ");
		m_vt->setColor(KVT_FGCOLOR);
		if (tokens[0] == "help") {
			*m_vt << " - Command list for integrated kernel shell:\n";
			*m_vt << "  - help       shows this help screen\n";
			*m_vt << "  - reboot     reboots your computer\n";
			*m_vt << "  - halt       shuts down your computer\n";
			*m_vt << "  - panic      causes a kernel panic\n";
			*m_vt << "  - devices    shows all detected devices on your computer\n";
			*m_vt << "  - loadkeys   loads specified kekymap\n";
			*m_vt << "  - free       shows memory usage (frames and kheap)\n";
			*m_vt << "  - uptime     shows seconds since boot\n";
			*m_vt << "  - part       shows all detected block devs and partitions\n";
			*m_vt << "  - Standard UNIX commands : ls cd cat pwd rm mkdir wf\n";
			*m_vt << " - Scroll up with shift+pgup !\n";
		} else if (tokens[0] == "reboot") {
			Sys::reboot();
		} else if (tokens[0] == "halt") {
			Sys::halt();
		} else if (tokens[0] == "panic") {
			PANIC("This is what happens when you say 'panic'.");
		} else if (tokens[0] == "exit") {
			if (tokens.size() == 1) return 0;
			return tokens[1].toInt();
		} else if (tokens[0] == "mount") {
			if (tokens.size() == 1) {
				for (u32int i = 0; i < VFS::filesystems.size(); i++) {
					*m_vt << VFS::path(VFS::filesystems[i]->getRootNode()) << "\n";
				}
			} else if (tokens.size() == 3) {
				if (tokens[1] == "ramfs") {
					FSNode* n = VFS::find(tokens[2], m_cwd);
					if (n == 0) {
						*m_vt << "No such directory.\n";
					} else if (n->type() != NT_DIRECTORY) {
						*m_vt << "Not a directory.\n";
					} else {
						if (RamFS::mount(100000, (DirectoryNode*)n) != 0) {
							*m_vt << "Ok...\n";
						} else {
							*m_vt << "Error !\n";
						}
					}
				} else {
					*m_vt << "Not supported yet.\n";
				}
			} else {
				*m_vt << "Usage: mount [<device> <mountpoint>\n";
			}
		} else if (tokens[0] == "unmount") {
			if (tokens.size() == 2) {
				FSNode* n = VFS::find(tokens[1], m_cwd);
				bool ok = false;
				if (n == 0) {
					ok = false;
				} else {
					String p = VFS::path(n);
					for (u32int i = 0; i < VFS::filesystems.size(); i++) {
						if (VFS::path(VFS::filesystems[i]->getRootNode()) == p) {
							VFS::unmount(VFS::filesystems[i]);
							ok = true;
							break;
						}
					}
				}
				if (ok) *m_vt << "Ok, filesystem unmounted.\n";
				else *m_vt << "Error.\n";
			} else {
				*m_vt << "Usage: unmount <mountpoint>\n";
			}
		} else if (tokens[0] != "" or tokens.size() != 1) {
			u32int i = 0;
			bool found = false;
			while (commands[i].name != 0) {
				if (tokens[0] == (const char*)commands[i].name) {
					found = true;
					if (commands[i].cmd != 0) {
						(this->*(commands[i].cmd))(tokens);	//Call command
					} else {
						*m_vt << "This command isn't enabled... yet !\n";
					}
					break;
				}
				i++;
			}
			if (!found) *m_vt << "Unknown command : " << tokens[0] << "\n";
		}
	}
	return 1337;
}
