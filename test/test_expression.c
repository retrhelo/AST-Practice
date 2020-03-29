#include <stdio.h>

void print(int a) {
	while (--a) 
		printf("hello world\n");
}

int main(void) {
	int a;
	a = 3 * 2 + 4 / 5;

	print(a);
	return a - 2;
}
