#include "KernelShell.class.h"
#include <VTManager/ScrollableVT.class.h>
#include <DeviceManager/Kbd.ns.h>
#include <SimpleList.class.h>
#include <MemoryManager/PhysMem.ns.h>
#include <TaskManager/Task.ns.h>
#include <VFS/VFS.ns.h>
#include <DeviceManager/Disp.ns.h>

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
		{"readblock", &KernelShell::readblock},
		{"mount", &KernelShell::mount},
		{"unmount", &KernelShell::unmount},
		{"hexdump", &KernelShell::hexdump},

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
			*m_vt << "  - mount      shows mounted devices or mounts a ramfs\n";
			*m_vt << "  - readblock  reads a block from a block device and dumps it\n";
			*m_vt << "  - hexdump    shows a hexadecimal dump of a file\n";
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
		} else if (tokens[0] == "mode") {
			bool ch = (m_vt == kvt);
			Disp::selectMode();
			if (ch) m_vt = kvt;
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
