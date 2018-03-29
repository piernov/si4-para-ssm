#include <math.h>
#include <stdio.h>
#include <limits.h>

int main() {
	int x = 0;

	int max = INT_MAX/128;
	for (unsigned int i = 0; i < max; i++) {
		x = log2(i);
	}
	printf("%d\n", x);
}
