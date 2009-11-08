#include <Binding/Process.class.h>
#include <String.class.h>

int main(Vector<String> args) {
	String act = "init";
	if (args.size() == 2) {
		if (args[1] == "login") {
			act = "login";
		} else if (args[1] == "init") {
			act = "init";
		}
	}

	if (act == "init") {
		while (1) {
			Process p = Process::run("/System/Applications/PaperWork.app");
			if (p.valid()) {
				p.setInVT(invt);
				p.setOutVT(outvt);
				p.pushArg("login");
				p.start();
				p.wait();	
			} else {
				return 1;
			}
		}
	} else if (act == "login") {
		outvt << "Logging in to Melon\n";
		String user, pw;
		while (1) {
			outvt << "Username : ";
			user = invt.readLine();
			outvt << "Password : ";
			pw = invt.readLine(false);
			if (!Process::get().authenticatePW(user, pw)) {
				outvt << "Authentication failed.\n\n";
				continue;
			}
			Process p = Process::run("/Applications/Shell/Shell.app");
			if (p.valid()) {
				p.setInVT(invt);
				p.setOutVT(outvt);
				p.start();
				p.wait();	
				outvt << "\n\n";
			} else {
				return 1;
			}
			return 0;
		}
	}
}
