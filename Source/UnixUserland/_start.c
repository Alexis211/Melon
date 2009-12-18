int main();

void _start() {
	main();
	asm volatile("int $66;");
	while(1);
}
