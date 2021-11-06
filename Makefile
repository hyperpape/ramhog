all:
	clang -g -fsanitize=undefined -std=c11 ramhog.c
