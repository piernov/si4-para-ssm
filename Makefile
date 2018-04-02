all: novac novac-mp

novac: novac.c
	gcc -std=c99 -g -Wall -Wextra -lm -o $@ $^

%-mp: %.c
	gcc -std=c99 -g -Wall -Wextra -lm -o $@ $^ -fopenmp

