#include <App/ShellApp.proto.h>
#include <Binding/FSNode.class.h>
#include <TextFile.class.h>

#define DEFAULT_SHELL "/Applications/Shell/Shell.app"
#define PAPERWORK_PATH "/System/Applications/PaperWork.app"

class PaperWork : public ShellApp {
	public:
	PaperWork() : ShellApp("PaperWork.app", "Melon's init/login manager") {
		addFlag("s", "shell", "Define the default shell to launch", FT_STR, DEFAULT_SHELL);
		addFlag("l", "login", "Act as a login manager");
		addFlag("i", "init", "Act as a init manager", FT_BOOL, "on");
	}
	int run();
};

APP(PaperWork);

int PaperWork::run() {
	String act = "init";
	if (bFlag("login")) act = "login";

	if (act == "init") {
		while (1) {
			Process p = Process::run(PAPERWORK_PATH);
			if (p.valid()) {
				p.setInVT(invt);
				p.setOutVT(outvt);
				p.pushArg("--login");
				p.start();
				p.wait();	
			} else {
				return 1;
			}
		}
	} else if (act == "login") {
		FSNode welcome = FS::find("/System/Configuration/Welcome");
		if (welcome.valid() && welcome.type() == NT_FILE) {
			TextFile f("/System/Configuration/Welcome");
			while (!f.eof()) outvt << f.readLine() << ENDL;
		} else {
			outvt << "Logging in to Melon\n";
		}
		String user, pw;
		while (1) {
			outvt << "Username: " << FLUSH;
			user = invt.get();
			outvt << "Password: " << FLUSH;
			pw = invt.readLine(false);
			if (!Process::get().authenticatePW(user, pw)) {
				outvt << "Authentication failed.\n\n";
				continue;
			}
			outvt << "What shell to run [" << sFlag("shell") << "]? "<< FLUSH;
			String sh = invt.get();
			if (sh == "") sh = sFlag("shell");
			Process p = Process::run(sh);
			if (p.valid()) {
				p.setInVT(invt);
				p.setOutVT(outvt);
				p.start();
				p.wait();	
				outvt << "\n\n";
			} else {
				outvt << "Could not run application : " << sh << ENDL;
				return 1;
			}
			return 0;
		}
	}
}
