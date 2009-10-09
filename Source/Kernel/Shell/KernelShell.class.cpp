#include "KernelShell.class.h"
#include <VTManager/ScrollableVT.class.h>
#include <DeviceManager/Kbd.ns.h>

u32int KernelShell::m_instances = 0;

#define COMMAND(name, wat) {(void*)name, (void*)(&KernelShell::wat)},

u32int shellRun(void* ks) {
	KernelShell* sh = (KernelShell*)ks;
	u32int ret = sh->run();
	delete sh;
	return ret;
}

KernelShell::KernelShell(DirectoryNode* cwd) {
	m_vt = new ScrollableVT(12, 76, 200, SHELL_FGCOLOR, SHELL_BGCOLOR);
	((ScrollableVT*)m_vt)->map(9);
	Kbd::setFocus(m_vt);
	m_cwd = cwd;
	*m_vt << "Welcome to Melon's kernel shell !\n";
	m_thread = new Thread(shellRun, (void*)this, true);
	m_instances++;
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

		{"devices", &KernelShell::devices},
		{"loadkeys", &KernelShell::loadkeys},
		{"free", &KernelShell::free},
		{"uptime", &KernelShell::uptime},
		{"part", &KernelShell::part},

		{0, 0}
	};
	
	while (1) {
		m_vt->setColor(SHELL_LIGHTCOLOR);
		*m_vt << "[" << m_cwd->getName() << "]# ";
		m_vt->setColor(SHELL_ENTRYCOLOR);
		Vector<String> tokens = m_vt->readLine().split(" ");
		m_vt->setColor(SHELL_FGCOLOR);
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
		} else if (tokens[0] != "" or tokens.size() != 1) {
			u32int i = 0;
			bool found = false;
			while (commands[i].name != 0) {
				if (tokens[0] == (const char*)commands[i].name) {
					found = true;
					if (commands[i].name != 0) {
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
