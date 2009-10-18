#include <Syscall/Syscall.wtf.h>
#include <WChar.class.h>

int main() {
	for (char c = ' '; c <= 'z'; c++) {
		syscall(0xFFFFFF02, (unsigned int)c);
		putch(c);
	}
	putch('\n');
}
