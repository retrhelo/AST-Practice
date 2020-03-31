#include <stdio.h>

void print(int a) {
	while (--a) 
		printf("hello world\n");
}

int main(void) {
	int a, b, c, d;
	a = 3 * 2 + 4 / 5;
	
	a = (b+c) / (c-d);
	a <<= (b & 0x0f);
	a = ++c - (--b + ++d);
	a = &b + 4;

	print(a);
	return a - 2;
}
