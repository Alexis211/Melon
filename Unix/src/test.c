#include <stdio.h>

int main() {
	printf("Hi, %d world !\n", 1337);
	char c[100];
	printf("Please enter your name: ");
	scanf("%s", c);
	printf("Your name is: %s\n", c);
	return 0;
}
