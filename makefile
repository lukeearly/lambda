# Lambda

parse: main.c parse.c parse.h
	cc -o parse main.c parse.c -I.
