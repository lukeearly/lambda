#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.h"

struct str input;
struct term *output;

int main(int argc, char *argv[]) {
	input.ptr = argv[1];
	input.len = strlen(argv[1]);
	printf("Input:          %s\n", argv[1]);
	switch (verify(input)) {
		case 1:
			printf("Verification failed; mismatched parentheses\n");
			return 1;
		case 2:
			printf("Verification failed; mismatched lambdas\n");
			return 1;
	}
	output = (struct term*)malloc(sizeof(struct term));
	parseTerm(input, output);
	printf("Tree:           %s\n", output->printT(output));
	printf("Reconstruction: %s\n", output->print(output));
	return 0;
}
