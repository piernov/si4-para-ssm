#include <math.h>
#include <stdio.h>
#include <limits.h>

  #define S(k) if (y >= (1 << k)) { x += k; y >>= k; }

int main() {
	int x = 0;
	int y = 0;

	int max = INT_MAX/128;
	for (unsigned int i = 0; i < max; i++) {
		x = 0;
	//	while (y >>= 1) {
	//		++x;
	//	}
	//	asm ( "\tbsr %1, %0\n"
	//		: "=r"(x)
	//		: "r" (i)
	//	);
		y = i;
		x = -(i == 0); S(16); S(8); S(4); S(2); S(1);

	}
	printf("%d\n", x);
}
