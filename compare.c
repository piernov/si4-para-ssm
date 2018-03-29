#include <math.h>
#include <stdio.h>
#include <limits.h>

int main() {
	int x = 0;
	for (unsigned int i = 0; i < UINT_MAX; i++) {
		x = 0;
		while (i >>= 1) ++x;
	}
	printf("%d\n", x);

	for (unsigned int i = 0; i < UINT_MAX; i++) {
		x = log2(i);
	}
	printf("%d\n", x);
}
