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

	do {
		a = b - 2;
		b = 2;
		if (a == b) {
			break;
		}
	} while (a != b);

	for (int a; b = 0; ++ i) {}

	return 0;
}
