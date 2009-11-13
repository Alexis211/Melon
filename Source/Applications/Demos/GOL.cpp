#include <Binding/VirtualTerminal.class.h>
#include <Binding/Thread.class.h>
#include <String.class.h>
#include <Rand.ns.h>

int main(Vector<String> args) {
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

	while (1) {
		//Display cells
		outvt.moveCursor(0, 0);
		for (u32int y = 0; y < h; y++) {
			for (u32int x = 0; x < w; x++) {
				if (cells[x * h + y]) {
					outvt.setColor(0, 2);
				} else {
					outvt.setColor(7, 0);
				}
				outvt << " ";
			}
		}
		outvt << "Press Ctrl+h for help";

		//Compute next generation
		for (u32int y = 0; y < h; y++) {
			for (u32int x = 0; x < w; x++) {
				u8int n = 0;
				for (u32int yy = y - 1; yy <= y + 1; yy++) {
					for (u32int xx = x - 1; xx <= x + 2; xx++) {
						if (xx < w and yy < h and cells[xx * h + yy]) n++;
					}
				}
				if (cells[x * h + y]) n--;
				if ((cells[x * h + y] and n == 2) or n == 3) {
					newcells[x * h + y] = true;
				} else {
					newcells[x * h + y] = false;
				}
			}
		}
		for (u32int x = 0; x < w; x++) {
			for (u32int y = 0; y < h; y++) {
				cells[x * h + y] = newcells[x * h + y];
			}
		}

		keypress_t kp = invt.getKeypress(false, false);
		if (kp.hascmd && (kp.modifiers & STATUS_CTRL)) {
			if (kp.character == WChar("c")) {
				break;
			} else if (kp.character == WChar("r")) {
				for (u32int i = 0; i < 20; i++) {
					u64int x = Rand::rand() * w / Rand::max();
					u64int y = Rand::rand() * h / Rand::max();
					cells[x * h + y] = true;
				}
			} else if (kp.character == WChar("R")) {
				for (u32int i = 0; i < h; i++) {
					cells[i] = true;
					cells[(2 * i) % (h - i)] = true;
					cells[(w * i) % (h * w - i)] = true;
				}
			} else if (kp.character == WChar("h")) {
				outvt << "** Melon's demo Game Of Life Simulator help :\n";
				outvt << " - ctrl+c : quit\n";
				outvt << " - ctrl+h : show this\n";
				outvt << " - ctrl+r : add some random cells\n";
				outvt << " - ctrl+R : add more cells, but not random\n";
				outvt << "Press any key to return to simultaor...";
				invt.getKeypress();
			}
		}

		Thread::get().sleep(100);
	}

	delete cells;
	delete newcells;
}
