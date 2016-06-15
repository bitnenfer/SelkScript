#ifndef SLKC_ERROR_H
#define SLKC_ERROR_H

#include <slkc/types.h>
#include <stdio.h>
#include <stdlib.h>

#define sk_emit_error(line, message, ...) { \
	fprintf(stderr, "SelkScript Error Line:%d ", line); \
	fprintf(stderr, ##message, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	getchar(); \
	exit(EXIT_FAILURE); \
}

#endif /* !SLKC_ERROR_H */