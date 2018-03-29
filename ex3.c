#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <limits.h>

struct tablo {
  int * tab; // long
  int size;
};

void printArray(struct tablo * tmp) {
  printf("---- Array of size %i ---- \n", tmp->size);
  int size = tmp->size;
  int i;
  for (i = 0; i < size; ++i) {
    printf("%i ", tmp->tab[i]);
  }
  printf("\n");
}

struct tablo * allocateTablo(int size) {
  struct tablo * tmp = malloc(sizeof(struct tablo));
  tmp->size = size;
  tmp->tab = malloc(size*sizeof(int));
  return tmp;
}

#define POW2(x) (1 << (x))
#define LOG2(x)	asm ( "\tbsr %1, %0\n" \
		: "=r"(x) \
		: "r" (x) \
	)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

unsigned int get_height(unsigned int size) {
	unsigned int m = size;
	LOG2(m);
	return m;
}

void montee(struct tablo * source, struct tablo * destination) {
	for (int i = source->size, j = destination->size; i >= 0; i--, j--) {
		destination->tab[j] = source->tab[i];
	}

	destination->tab[0] = 0;
         
	unsigned int m = get_height(source->size);
	
	for (int i = m - 1; i >= 0; i--) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			destination->tab[j/*parent*/] = destination->tab[2*j /*fils gauche*/] + destination->tab[2*j+1 /*fils droit*/];
		}
	}
}

void monteeMax(struct tablo * source, struct tablo * destination) {
	for (int i = source->size, j = destination->size; i >= 0; i--, j--) {
		destination->tab[j] = source->tab[i];
	}

	destination->tab[0] = 0;
         
	unsigned int m = get_height(source->size);
	
	for (int i = m - 1; i >= 0; i--) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			destination->tab[j/*parent*/] = MAX( destination->tab[2*j /*fils gauche*/] , destination->tab[2*j+1 /*fils droit*/] );
		}
	}
}

void descente(struct tablo * a, struct tablo * b) {
	b->tab[1] = 0;

	unsigned int m = get_height(a->size);
	
	for (int i = 1; i <= m; i++) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			if (j%2 == 0 /*pair, fils gauche*/)
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			else
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/] + a->tab[j-1];
		}
	}
}

void descenteSuff(struct tablo * a, struct tablo * b) {
	b->tab[1] = 0;

	unsigned int m = get_height(a->size);
	
	for (int i = 1; i < m; i++) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			if (j%2 == 1 /*pair, fils droit*/)
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			else
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/] + a->tab[j+1];
		}
	}
}

void descentePreMax(struct tablo * a, struct tablo * b) {
	b->tab[1] = INT_MIN;

	unsigned int m = get_height(a->size);
	
	for (int i = 1; i <= m; i++) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			if (j%2 == 0 /*pair, fils gauche*/)
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			else
				b->tab[j/*nœud*/] = MAX( b->tab[j/2/*parent*/] , a->tab[j-1] );
		}
	}
}

void descenteSuffMax(struct tablo * a, struct tablo * b) {
	b->tab[1] = INT_MIN;

	unsigned int m = get_height(a->size);
	
	for (int i = 1; i < m; i++) {
		for (int j = POW2(i); j <= POW2(i+1) -1; j++) {
			if (j%2 == 1 /*pair, fils droit*/)
				b->tab[j/*nœud*/] = b->tab[j/2/*parent*/];
			else
				b->tab[j/*nœud*/] = MAX(b->tab[j/2/*parent*/] , a->tab[j+1]);
		}
	}
}

void final(struct tablo * a, struct tablo *b) {
	unsigned int m = get_height(a->size/2);

	for (int i = POW2(m); i <= POW2(m+1) -1; i++) {
		b->tab[i] = b->tab[i] + a->tab[i];
	}

}

void finalMax(struct tablo * a, struct tablo *b) {
	unsigned int m = get_height(a->size/2);

	for (int i = POW2(m); i <= POW2(m+1) -1; i++) {
		b->tab[i] = MAX( b->tab[i] , a->tab[i]);
	}

}

void generateArray(struct tablo * s) {
  //construction d'un tableau pour tester
  //s->size=8;
  s->size = 16;
  s->tab=malloc(s->size*sizeof(int));
/*  s->tab[0]=3;
  s->tab[1]=1;
  s->tab[2]=7;
  s->tab[3]=0;
  s->tab[4]=4;
  s->tab[5]=1;
  s->tab[6]=6;
  s->tab[7]=3;
  */
  s->tab[0] = 3;
  s->tab[1] = 2;
  s->tab[2] = -7;
  s->tab[3] = 11;
  s->tab[4] = 10;
  s->tab[5] = -6;
  s->tab[6] = 4;
  s->tab[7] = 9;
  s->tab[8] = -6;
  s->tab[9] = 1;
  s->tab[10] = -2;
  s->tab[11] = -3;
  s->tab[12] = 4;
  s->tab[13] = -3;
  s->tab[14] = 0;
  s->tab[15] = 2;

}

int main(int argc, char **argv) {
  struct tablo source;

  /* read input => Q */

  /* 1: sum-prefix Q => PSUM */
  printf("\nStep1:\n");
  generateArray(&source);
  printArray(&source);

  struct tablo * a = allocateTablo(source.size*2);
  montee(&source, a);
  printArray(a);

  struct tablo * b = allocateTablo(source.size*2);
  descente(a, b);
  printArray(b);
   
  final(a,b);
  printArray(b);



  /* 2: sum-suffix Q => SSUM */
  printf("\nStep 2:\n");
  struct tablo * b2 = allocateTablo(source.size*2);
  descenteSuff(a, b2);
  printArray(b2);
   
  final(a,b2);
  printArray(b2);



  /* 3: max-suffix PSUM => SMAX */
  printf("\nStep3:\n");
  struct tablo * apm = allocateTablo(source.size*2);
  struct tablo btmp = *b;
  btmp.size /= 2;
  btmp.tab = btmp.tab + source.size;
  monteeMax(&btmp, apm);
  printArray(apm);

  struct tablo * bpm = allocateTablo(source.size*2);
  descenteSuffMax(apm, bpm);
  printArray(bpm);
   
  finalMax(apm,bpm);
  printArray(bpm);

  /* 4: max-prefix SSUM => PMAX */
  printf("\nStep4:\n");
  struct tablo * assm = allocateTablo(source.size*2);
  struct tablo btmp2 = *b2;
  btmp2.size /= 2;
  btmp2.tab = btmp2.tab + source.size;
  monteeMax(&btmp2, assm);
  printArray(assm);

  struct tablo * bssm = allocateTablo(source.size*2);
  descentePreMax(assm, bssm);
  printArray(bssm);
   
  finalMax(assm,bssm);
  printArray(bssm);

  /* 5: for (int i = 0; i < n; i++)
   * 	1: Ms[i] = PMAX[i] - SSUM[i] + Q[i]
   * 	2: Mp[i] = SMAX[i] - PSUM[i] + Q[i]
   * 	3: M[i] = Ms[i] + Mp[i] - Q[i] */

  printf("\nStep5:\n");

  //parrallel
  	struct tablo * Ms = allocateTablo(source.size);
	struct tablo * Mp = allocateTablo(source.size);
	struct tablo * M = allocateTablo(source.size);
	for (int i = source.size; i <= 2*source.size; i++) {
		Ms->tab[i - source.size] = bpm->tab[i] - b2->tab[i] + source.tab[i - source.size];
		Mp->tab[i - source.size] = bssm->tab[i] - b->tab[i] + source.tab[i - source.size];
		M->tab[i - source.size] = Ms->tab[i - source.size] + Mp->tab[i - source.size] - source.tab[i - source.size];
	}
	printArray(M);
		
  /* 6: max in M */
  printf("\nStep6:\n");
  long max = INT_MIN;
  int ai = -1;
  int bi = -1;
  int inseq = 0;
  for (int i = 0; i < M->size; i++) {
	if (M->tab[i] >= max) {
		if (M->tab[i] > max) {
			max = M->tab[i];
			inseq = 0;
		}

		printf("max %d %d\n", max, inseq);
		if (ai == -1 || !inseq) {
			printf("miaou %d\n", max);
			ai = i;
		}
		if (bi == -1 || !inseq)
			bi = i;
		else
			bi++;
		inseq = 1;
		
	}
	else {
		printf("meow\n");
		inseq = 0;
	}
  }
  printf("max: %d, i %d j %d\n", max, ai+1, bi+1);

  /* print output MSS */
}
