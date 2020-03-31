#include <stdio.h>

/* block comment line 1
	block comment line 2
	block comment line 3
*/

// line comment 1
// line comment 2

void func(void);

extern int alpha;
extern double beta;

struct s {
	int a;
};

int main(void) {
	// keyword match
	auto char a;
	const int b1;
	int b2;
	static double c;
	volatile long d;
	signed e;
	unsigned f;
	float g;
	char *cptr;
	void **p;
	struct s ss;
	
	a = 'a';
	c = 3.14;
	g = 3.14f;
	f = 0xff;
	b2 = 0123;
	b2 = 123l;
	b2 = 123;
	e = -123;
	
	// punctuations and operators
	cptr = &a;
	++ a;
	-- b2;
	a += b2;
	a = a + b2;
	b2 = b2 - a;
	b2 -= a;
	b2 = a*b2;
	b2 *= a;
	b2 = a / b2;
	b2 /= a;
	b2 = b2 % a;
	b2 %= a;
	b2 = a >> 2;
	a >>= 2;
	b2 = a << 2;
	b2 <<= 2;
	b2 = a & 0x0f;
	b2 &= 0x0f;
	b2 = a | 0x0f;
	b2 |= 0x0f;
	b2 = a ^ 0xff;
	b2 ^= 0xff;
	b1 == a;
	b1 != a;
	b1 > a;
	b1 >= a;
	b1 < a;
	b1 <= a;
	b2 = a && b1;
	b2 = a || b1;
	b2 = !a;
	b2 = a+1, b1+2;
	
	// blocks 
	if (a) {/* codes here */}
	else {/* codes here */}
	
	while (a) {}
	do {
		continue;
	} while (a);
	for (int x; a; a) {
		break;
	}
	
	return 0;
}