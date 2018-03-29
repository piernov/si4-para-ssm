all: ex3 ex3-mp

ex%: ex%.c
	gcc -g -Wall -Wextra -lm -o $@ $^

%-mp: %.c
	gcc -g -Wall -Wextra -lm -o $@ $^ -fopenmp

