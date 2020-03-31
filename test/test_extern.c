#include <stdio.h>

#define hello

extern int a;

static void foo1(int a);
int foo2(void);

static void foo1(int a) {
	while (-- a) 
		printf("helloworld %d\n", a);
}

struct s {
	int a;
	double b;
};

union u {
	struct s tmp;
	int a;
};

enum e {
	ONE, TWO, THREE,
}

int main(void) {
	int a;
	struct s st;
	union u un;
	enum e en;
	
	a = 4;
	st.a = foo2();
	
	foo1(a);
	
	return 0;
}