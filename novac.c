#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

struct tablo {
  long * tab; // long
  size_t size;
};

struct maxTreeNode {
	long l;
	long v;
	long r;
};

void printArray(struct tablo * tmp) {
	size_t size = tmp->size;
	size_t i;
	for (i = 0; i < size; ++i) {
		printf("%li ", tmp->tab[i]);
	}
	printf("\n");
}

struct tablo allocateTablo(size_t size) {
	struct tablo tmp;
	tmp.size = size;
	tmp.tab = malloc(size*sizeof(long));
	return tmp;
}

void freeTablo(struct tablo *tab) {
	free(tab->tab);
	tab->tab = NULL;
	tab->size = 0;
}

#define POW2(x) ((unsigned long)(1 << (x)))
#ifdef __x86_64__
#define LOG2(x)	__asm__ ( "\tbsr %1, %0\n" \
		: "=r"(x) \
		: "r" (x) \
	)
#else
#define LOG2(x) x = (unsigned long)(log2(x))
#endif
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

unsigned long get_height(long size) {
	unsigned long m = size;
	LOG2(m);
	return m;
}

void fill_dest(struct tablo *source, struct tablo *destination) {
#pragma omp parallel for
	for (size_t i = source->size; i > 0; i--) {
		size_t j = source->size + i;
		destination->tab[j - 1] = source->tab[i - 1];
	}

	destination->tab[0] = 0;
}

void montee(struct tablo * source, struct tablo * destination) {
	fill_dest(source, destination);

	unsigned long m = get_height(source->size);
	
	for (size_t i = m; i != 0; i--) {
		const size_t jend = POW2(i) - 1;
#pragma omp parallel for
		for (size_t j = POW2(i - 1); j <= jend; j++) {
			destination->tab[j/*parent*/] = destination->tab[2*j /*fils gauche*/] + destination->tab[2*j+1 /*fils droit*/];
		}
	}
}

void monteeMax(struct tablo * source, struct tablo * destination) {
	fill_dest(source, destination);

	unsigned long m = get_height(source->size);
	
	for (size_t i = m; i != 0; i--) {
		const size_t jend = POW2(i) - 1;
#pragma omp parallel for
		for (size_t j = POW2(i - 1); j <= jend; j++) {
			destination->tab[j/*parent*/] = MAX( destination->tab[2*j /*fils gauche*/] , destination->tab[2*j+1 /*fils droit*/] );
		}
	}
}

void descente(struct tablo * a, struct tablo * b) {
	b->tab[1] = 0;

	size_t m = get_height(a->size);
	
	for (size_t i = 1; i < m; i++) {
		const size_t jend = POW2(i) - 1;
#pragma omp parallel for
		for (size_t oj = POW2(i - 1); oj <= jend; oj++) {
			size_t j = oj << 1;
			b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			b->tab[j+1/*nœud*/] = b->tab[(j+1)/2/*parent*/] + a->tab[(j+1)-1];
		}
	}
}

void descenteSuff(struct tablo * a, struct tablo * b) {
	b->tab[1] = 0;

	unsigned int m = get_height(a->size);
	
	for (size_t i = 1; i < m; i++) {
		const size_t jend = POW2(i) - 1;
#pragma omp parallel for
		for (size_t oj = POW2(i-1); oj <= jend; oj++) {
			size_t j = oj << 1;
			b->tab[j/*nœud*/] = b->tab[j/2/*parent*/] + a->tab[j+1];
			b->tab[j+1/*nœud*/] = b->tab[(j+1)/2/*parent*/];
		}
	}
}

void descentePreMax(struct tablo * a, struct tablo * b) {
	b->tab[1] = LONG_MIN;

	size_t m = get_height(a->size);
	
	for (size_t i = 1; i < m; i++) {
		const size_t jend = POW2(i)-1;
#pragma omp parallel for
		for (size_t oj = POW2(i-1); oj <= jend; oj++) {
			size_t j = oj << 1;
			b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			b->tab[j+1/*nœud*/] = MAX( b->tab[(j+1)/2/*parent*/] , a->tab[(j+1)-1] );
		}
	}
}

void descenteSuffMax(struct tablo * a, struct tablo * b) {
	b->tab[1] = LONG_MIN;

	size_t m = get_height(a->size);
	
	for (size_t i = 1; i < m; i++) {
		const size_t jend = POW2(i)-1;
#pragma omp parallel for
		for (size_t oj = POW2(i-1); oj <= jend; oj++) {
			size_t j = oj << 1;
			b->tab[j/*nœud*/] = MAX(b->tab[j/2/*parent*/] , a->tab[j+1]);
			b->tab[j+1/*nœud*/] = b->tab[(j+1)/2/*parent*/];
		}
	}
}

void final(struct tablo * a, struct tablo *b) {
	size_t m = get_height(a->size/2);

	const size_t iend = POW2(m+1) - 1;
#pragma omp parallel for
	for (size_t i = POW2(m); i <= iend; i++) {
		b->tab[i] = b->tab[i] + a->tab[i];
	}

}

void finalMax(struct tablo * a, struct tablo *b) {
	size_t m = get_height(a->size/2);

	const size_t iend = POW2(m+1) - 1;
#pragma omp parallel for
	for (size_t i = POW2(m); i <= iend; i++) {
		b->tab[i] = MAX( b->tab[i] , a->tab[i]);
	}

}

struct tablo buildOut(struct tablo source, struct tablo ssum, struct tablo psum, struct tablo smax, struct tablo pmax) {
  	struct tablo Ms = allocateTablo(source.size);
	struct tablo Mp = allocateTablo(source.size);
	struct tablo M = allocateTablo(source.size);

	const size_t iend = 2*source.size;
#pragma omp parallel for
	for (size_t i = source.size; i < iend; i++) {
#pragma omp parallel
{
		Ms.tab[i - source.size] = pmax.tab[i] - ssum.tab[i] /*+ source.tab[i - source.size]*/;
		Mp.tab[i - source.size] = smax.tab[i] - psum.tab[i] /*+ source.tab[i - source.size]*/;
}
		M.tab[i - source.size] = Ms.tab[i - source.size] + Mp.tab[i - source.size] /*-*/ + source.tab[i - source.size];
	}
	freeTablo(&Ms);
	freeTablo(&Mp);
	return M;
}

struct maxTreeNode findMax(struct tablo source) {
	struct maxTreeNode *maxTree = malloc(source.size * 2 * sizeof(struct maxTreeNode));
#pragma omp parallel for
	for (size_t i = source.size; i > 0; i--) {
		size_t j = source.size + i;
		struct maxTreeNode *n = maxTree + j - 1;
		n->v = source.tab[i - 1];
		n->l = i - 1; 
		n->r = i;
	}

	maxTree[0].v = 0;
	unsigned int m = get_height(source.size);

	for (size_t i = m; i != 0; i--) {
		const size_t jend = POW2(i) - 1;
#pragma omp parallel for
		for (size_t j = POW2(i - 1); j <= jend; j++) {
			struct maxTreeNode *l = maxTree + (2*j) /*fils gauche*/;
			struct maxTreeNode *r = maxTree + (2*j+1) /*fils droit*/;
			struct maxTreeNode *p = maxTree + j;
			p->v = MAX( l->v , r->v );
			//branch predictor will probably hate this
			if (l->v > r->v) {
				p->l = l->l;
				p->r = l->r;
			}
			else if (l->v < r->v) {
				p->l = r->l;
				p->r = r->r;
			}
			else { // max equal so we need to check for a break in the sequence further down
				if (l->r != r->l) {
					if (l->r - l->l > r->r - r->l) { // longest sequence on the left
						p->l = l->l;
						p->l = l->r;
					}
					else { //longest sequence on the right
						p->l = r->l;
						p->l = r->r;
					}
				}
				else { //merge sequences
					p->l = l->l;
					p->r = r->r;
				}
			}
		}
	}
	struct maxTreeNode maxNode = maxTree[1];
	free(maxTree);
	return maxNode;
}

void readArray(const char *filename, struct tablo *tab) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed: %s\n", filename, strerror (errno));
	}

	struct stat s;
	int status = fstat(fd, &s);
	if (status < 0) {
		fprintf(stderr, "stat %s failed: %s\n", filename, strerror (errno));
	}
	size_t size = s.st_size;

	char *mapped = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED) {
		fprintf(stderr, "mmap %s failed: %s\n", filename, strerror (errno));
	}

	unsigned long arraySize = 131072;
	long *array = malloc(arraySize * sizeof(long));
	if (!array) exit(31);
	size_t index = 0;

	char *endptr = mapped;
	while (endptr < mapped + size - 1) {
		long value = strtol(endptr, &endptr, 10);
		array[index] = value;
		index++;
		if (index >= arraySize) {
			arraySize = arraySize * 3 / 2;
			array = realloc(array, arraySize * sizeof(long));
			if (!array) exit(32);
		}
	}
	tab->tab = array;
	tab->size = index;
}

int main(int argc, char **argv) {
	if (argc < 2) exit(3); // must specify input file

	/* read input => Q */
	struct tablo source;
	readArray(argv[1], &source);

	/* 1: sum-prefix Q => PSUM */
	struct tablo a = allocateTablo(source.size*2);
	montee(&source, &a);
	struct tablo psum = allocateTablo(source.size*2);
	descente(&a, &psum);
	final(&a, &psum);

	/* 2: sum-suffix Q => SSUM */
	struct tablo ssum = allocateTablo(source.size*2);
	descenteSuff(&a, &ssum);
	final(&a, &ssum);

	freeTablo(&a);

	/* 3: max-suffix PSUM => SMAX */
	struct tablo apm = allocateTablo(source.size*2);
	struct tablo btmp = psum;
	btmp.size /= 2;
	btmp.tab = btmp.tab + source.size;
	monteeMax(&btmp, &apm);

	struct tablo smax = allocateTablo(source.size*2);
	descenteSuffMax(&apm, &smax);
	finalMax(&apm, &smax);

	freeTablo(&apm);

	/* 4: max-prefix SSUM => PMAX */
	struct tablo assm = allocateTablo(source.size*2);
	struct tablo btmp2 = ssum;
	btmp2.size /= 2;
	btmp2.tab = btmp2.tab + source.size;
	monteeMax(&btmp2, &assm);

	struct tablo pmax = allocateTablo(source.size*2);
	descentePreMax(&assm, &pmax);
	finalMax(&assm, &pmax);

	freeTablo(&assm);

	/* 5: for (int i = 0; i < n; i++)
	 * 	1: Ms[i] = PMAX[i] - SSUM[i] + Q[i]
	 * 	2: Mp[i] = SMAX[i] - PSUM[i] + Q[i]
	 * 	3: M[i] = Ms[i] + Mp[i] - Q[i] */
	struct tablo M = buildOut(source, ssum, psum, smax, pmax);
	freeTablo(&ssum);
	freeTablo(&psum);
	freeTablo(&smax);
	freeTablo(&pmax);

	/* 6: max in M */
	struct maxTreeNode maxNode = findMax(M);
	freeTablo(&M);

	long max = maxNode.v;
	size_t ai = maxNode.l;
	size_t bi = maxNode.r;

	/* 7: print output MSS */
	printf("%ld ", max);
	struct tablo out = source;
	out.size = bi - ai;
	out.tab += ai;
	printArray(&out);
	freeTablo(&source);
}
