all: ex3 ex3-mp

ex%: ex%.c
	gcc -std=c99 -g -Wall -Wextra -lm -o $@ $^

%-mp: %.c
	gcc -std=c99 -g -Wall -Wextra -lm -o $@ $^ -fopenmp

