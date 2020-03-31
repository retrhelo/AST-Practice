#include <stdio.h>

int main(void) {
	int a, b;

	a = b = 1;
	if (a = b) {
		printf("hello world\n");
	}
	else if (a != b) {
		printf("hello world\n");
	}

	while (a != b) {
		printf("hello world");
		a = 2;
		++ b;
	}
	
	do {} while(1);

	for (int a; b = 0; ++ i) {}

	do {
		a = b - 2;
		b = 2;
		if (a == b) 
			-- a;
		else if (a > b) 
			++ a;
		else {
			if (a > c) 
				printf("hello world\n");
			else {
				while (a < c) {
					printf("hello world\n");
					for (int a; a < c; ++ a) 
						printf("world hello\n");
				}
			}
		}
	} while (a != b);

	return 0;
}
