#include <Binding/Thread.class.h>
#include <ByteArray.class.h>
#include <Rand.ns.h>

#include <App/ShellApp.proto.h>

class GOL : public ShellApp {
	public:
	GOL() : ShellApp("GOL.app", "Melon's Game of Life simulator \\o/") {}
	int run();
};

APP(GOL);

int GOL::run() {
	if (!outvt.isBoxed()) {
		outvt << "Error : cannot display GOL on a non-boxed terminal.\n";
		return 1;
	}

	int h = outvt.height() - 1, w = outvt.width();

	bool *cells = new bool[w * h];
	bool *newcells = new bool[w * h];

	u32int delay = 100;

	for (u32int x = 0; x < w; x++) {
		for (u32int y = 0; y < h; y++) {
			cells[x * h + y] = false;
		}
	}

	ByteArray tmp((w + 1) * (h + 1));

	bool run = true;
	while (run) {
		//Display cells
		for (u32int y = 0; y < h; y++) {
			for (u32int x = 0; x < w; x++) {
				if (cells[x * h + y]) {
					tmp[y * w + x] = '#';
				} else {
					tmp[y * w + x] = ' ';
				}
			}
		}
		outvt << MVT::movecsr(0, 0);
		outvt.write(tmp.toString());	//BYPASS buffering
		outvt<< "Press Ctrl+h for help" << FLUSH;

		//Compute next generation
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				u8int n = 0;
				for (int yy = y - 1; yy < y + 2; yy++) {
					for (int xx = x - 1; xx < x + 2; xx++) {
						if (xx < w and yy < h and xx >= 0 and yy >= 0
							   and cells[xx * h + yy]) n++;
					}
				}
				newcells[x * h + y] = cells[x * h + y];
				if (cells[x * h + y]) {
					n--;
					if (n < 2) newcells[x * h + y] = false;
					if (n > 3) newcells[x * h + y] = false;
				} else {
					if (n == 3) newcells[x*  h + y] = true;
				}

			}
		}
		for (u32int x = 0; x < w; x++) {
			for (u32int y = 0; y < h; y++) {
				cells[x * h + y] = newcells[x * h + y];
			}
		}

		keypress_t kp = invt.getKeypress(false, false);
		while (kp.hascmd && (kp.modifiers & STATUS_CTRL)) {
			if (kp.character == WChar("c")) {
				run = false;
			} else if (kp.character == WChar("r")) {
				for (u32int i = 0; i < 20; i++) {
					u64int x = Rand::rand() * w / Rand::max();
					u64int y = Rand::rand() * h / Rand::max();
					cells[x * h + y] = true;
				}
			} else if (kp.character == WChar("R")) {
				for (u32int i = 0; i < 150; i++) {
					u64int x = Rand::rand() * w / Rand::max();
					u64int y = Rand::rand() * h / Rand::max();
					cells[x * h + y] = true;
				}
			} else if (kp.character == WChar("p")) {
				outvt << " [PAUSED] press a key to resume" << FLUSH;
				invt.getKeypress();
			} else if (kp.character == WChar("h")) {
				outvt << "\n\n** Melon's demo Game Of Life Simulator help :\n";
				outvt << " - ctrl+c : quit\n";
				outvt << " - ctrl+h : show this\n";
				outvt << " - ctrl+p : pause\n";
				outvt << " - ctrl+r : add some random cells\n";
				outvt << " - ctrl+R : add more cells, still random\n\n";
				outvt << "Press any key to return to simultaor..." << FLUSH;
				invt.getKeypress();
			}
			kp = invt.getKeypress(false, false);
		}

		Thread::get().sleep(100);
	}

	delete cells;
	delete newcells;

	outvt << "\n\n";
}
