int write(int file, char *ptr, int len) {
	int i;
	for (i = 0; i < len; i++) {
		int t = ptr[i];
		asm volatile("mov $0xFFFFFF01, %%eax;		\
				mov %0, %%ebx;						\
				int $64;" : : "r"(t));
	}
	return len;
}
